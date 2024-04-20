#include "Engine.hpp"

#include <chrono>
#include <thread>

#include <SDL.h>
#include <SDL_vulkan.h>

#include "vkBootstrap.h"


#include <lud_assert.hpp>

#include "Vulkan/Descriptors.hpp"
#include "Vulkan/Initializers.hpp"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>


#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_vulkan.h>

#include "ImGui/OpenSans-Regular.embed"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/transform.hpp>


namespace Ui
{

using namespace Detail;

Engine* loaded_engine = nullptr;

Engine& Engine::Get()
{
	return *loaded_engine;
}

Engine::Engine()
{
	init();
}

Engine::~Engine()
{
	cleanup();
}

void Engine::init()
{
	Lud::assert::eq(loaded_engine, nullptr);

	loaded_engine = this;

	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(
		SDL_WINDOW_VULKAN |
		SDL_WINDOW_RESIZABLE
		);

	m_window = SDL_CreateWindow(
		"Vulkan engine",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		m_window_extent.width,
		m_window_extent.height,
		window_flags
	);
	Lud::assert::ne(m_window, nullptr);

	init_vulkan();

	init_swapchain();

	init_commands();

	init_sync_structures();

	init_descriptors();

	init_pipelines();

	init_imgui();

	init_default_data();

	m_initialised = true;
}


void Engine::draw()
{
	VK_CHECK(m_device.waitForFences(get_current_frame().render_fence, true, 1'000'000'000));

	get_current_frame().deletion_queue.Flush();
	get_current_frame().frame_descriptor.ClearPools(m_device);

	u32 swapchain_image_index;

	try
	{
		const auto [err, val] = m_device.acquireNextImageKHR(m_swapchain, 1'000'000'000, get_current_frame().swapchain_semaphore);
		VK_CHECK(err);
		swapchain_image_index = val;
	} catch (const vk::OutOfDateKHRError&)
	{
		m_resize_requested = true;
		return;
	}

	m_draw_extent.width = static_cast<u32>( std::min(m_swapchain.extent.width, m_draw_image.extent.width) * m_render_scale );
	m_draw_extent.height = static_cast<u32>( std::min(m_swapchain.extent.height, m_draw_image.extent.height) * m_render_scale );

	m_device.resetFences(get_current_frame().render_fence);
	get_current_frame().command_buffer.reset();

	vk::CommandBuffer cmd = get_current_frame().command_buffer;


	vk::CommandBufferBeginInfo cmd_begin_info = vkinit::command_buffer_begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	cmd.begin(cmd_begin_info);

	vkutil::transition_image(
		cmd,
		m_draw_image.image,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eGeneral
	);



	draw_background(cmd);

	vkutil::transition_image(
		cmd,
		m_draw_image.image,
		vk::ImageLayout::eGeneral,
		vk::ImageLayout::eColorAttachmentOptimal
	);
	vkutil::transition_image(
		cmd,
		m_depth_image.image,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eDepthAttachmentOptimal
	);

	draw_geometry(cmd);


	vkutil::transition_image(
		cmd,
		m_draw_image.image,
		vk::ImageLayout::eColorAttachmentOptimal,
		vk::ImageLayout::eTransferSrcOptimal
	);
	vkutil::transition_image(
		cmd,
		m_swapchain.frames[swapchain_image_index].image,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eTransferDstOptimal
	);
	vkutil::copy_image_to_image(
		cmd,
		m_draw_image.image,
		m_swapchain.frames[swapchain_image_index].image,
		m_draw_extent,
		m_swapchain.extent
	);

	vkutil::transition_image(
		cmd,
		m_swapchain.frames[swapchain_image_index].image,
		vk::ImageLayout::eTransferDstOptimal,
		vk::ImageLayout::eColorAttachmentOptimal
	);

	draw_imgui(cmd, m_swapchain.frames[swapchain_image_index].view);

	vkutil::transition_image(
		cmd,
		m_swapchain.frames[swapchain_image_index].image,
		vk::ImageLayout::eColorAttachmentOptimal,
		vk::ImageLayout::ePresentSrcKHR
	);

	cmd.end();

	auto cmd_info = vkinit::command_buffer_submit_info(cmd);
	auto wait_info = vkinit::semaphire_submit_info(vk::PipelineStageFlagBits2::eColorAttachmentOutput, get_current_frame().swapchain_semaphore);
	auto signal_info = vkinit::semaphire_submit_info(vk::PipelineStageFlagBits2::eAllGraphics, get_current_frame().render_semaphore);
	auto submit = vkinit::submit_info(&cmd_info, &signal_info, &wait_info);

	m_graphics_queue.queue.submit2(submit, get_current_frame().render_fence);

	// prepare present
	vk::PresentInfoKHR present_info(get_current_frame().render_semaphore, m_swapchain.swapchain, swapchain_image_index);


	try
	{
		VK_CHECK(m_graphics_queue.queue.presentKHR(present_info));
	} catch (const vk::OutOfDateKHRError&)
	{
		m_resize_requested = true;
		return;
	}

	m_frame_number++;
}

void Engine::draw_background(vk::CommandBuffer cmd)
{
	ComputeEffect& effect = m_background_effects[m_current_background_effect];

	cmd.bindPipeline(vk::PipelineBindPoint::eCompute, effect.pipeline);

	cmd.bindDescriptorSets(vk::PipelineBindPoint::eCompute, m_gradient_pipeline_layout, 0, m_draw_image_descriptors, nullptr);

	cmd.pushConstants(m_gradient_pipeline_layout, vk::ShaderStageFlagBits::eCompute, 0, sizeof(effect.data), &effect.data);

	cmd.dispatch(static_cast<u32>( std::ceil(m_draw_extent.width / 16.0) ), static_cast<u32>( std::ceil(m_draw_extent.height / 16.0) ), 1);
}

void Engine::draw_geometry(vk::CommandBuffer cmd)
{

	vk::RenderingAttachmentInfo color_attachment = vkinit::attachment_info(m_draw_image.view, nullptr, vk::ImageLayout::eGeneral);
	vk::RenderingAttachmentInfo depth_attachment = vkinit::depth_attachment_info(m_depth_image.view, vk::ImageLayout::eDepthAttachmentOptimal);

	vk::RenderingInfo render_info = vkinit::rendering_info(m_draw_extent, &color_attachment, &depth_attachment);

	cmd.beginRendering(render_info);

	cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_mesh_pipeline);

	vk::Viewport viewport(0, 0, static_cast<float>( m_draw_extent.width ), static_cast<float>( m_draw_extent.height ), 0.0f, 1.0f);

	cmd.setViewport(0, viewport);

	vk::Rect2D scissor({ 0, 0 }, { m_draw_extent.width, m_draw_extent.height });

	cmd.setScissor(0, scissor);


	vk::DescriptorSet image_set = get_current_frame().frame_descriptor.Allocate(m_device, m_single_image_descriptor_layout);

	{
		auto writer = DescriptorWriter();
		writer
			.WriteImage(0, m_error_checkerboard_image.view, m_default_sampler_nearest, vk::ImageLayout::eShaderReadOnlyOptimal, vk::DescriptorType::eCombinedImageSampler)
			.UpdateSet(m_device, image_set);
	}

	cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_mesh_pipeline_layout, 0, image_set, {});

	glm::mat4 view = glm::translate(glm::vec3{ 0, 0, -5 });

	glm::mat4 projection = glm::perspective(glm::radians(70.0f), static_cast<float>( m_draw_extent.width ) / static_cast<float>( m_draw_extent.height ), 10000.0f, 0.1f);

	projection[1][1] *= -1;


	GPUDrawPushConstants push_constants{};
	push_constants.world_matrix = projection * view;
	push_constants.vertex_buffer = m_test_meshes[2]->mesh_buffers.address;

	cmd.pushConstants(m_mesh_pipeline_layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(GPUDrawPushConstants), &push_constants);

	cmd.bindIndexBuffer(m_test_meshes[2]->mesh_buffers.index_buffer.buffer, 0, vk::IndexType::eUint32);

	cmd.drawIndexed(m_test_meshes[2]->surfaces[0].count, 1, m_test_meshes[2]->surfaces[0].start_index, 0, 0);

	// TODO: cache

	/*auto gpu_scene_buffer = create_buffer(sizeof(GPUSceneData), vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU);
	GPUSceneData* scene_uniform_data = std::bit_cast<GPUSceneData*>( gpu_scene_buffer.allocation->GetMappedData() );
	*scene_uniform_data = m_scene_data;

	auto global_descriptor = get_current_frame().frame_descriptor.Allocate(m_device, m_scene_data_descriptor_layout);

	get_current_frame().deletion_queue.PushFunction([=, this]
		{
			destroy_buffer(gpu_scene_buffer);
		});


	DescriptorWriter()
		.WriteBuffer(0, gpu_scene_buffer.buffer, sizeof(GPUSceneData), 0, vk::DescriptorType::eUniformBuffer)
		.UpdateSet(m_device, global_descriptor);*/

	cmd.endRendering();
}

void Engine::resize_swapchain()
{
	m_device.waitIdle();

	destroy_swapchain();

	int w, h;
	SDL_GetWindowSize(m_window, &w, &h);
	m_window_extent.width = w;
	m_window_extent.height = h;

	create_swapchain(m_window_extent.width, m_window_extent.height);

	m_resize_requested = false;
}


void Engine::Run()
{
	bool should_quit = false;

	SDL_Event event;
	while (!should_quit)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				should_quit = true;

			if (event.type == SDL_WINDOWEVENT)
			{
				if (event.window.event == SDL_WINDOWEVENT_MINIMIZED)
				{
					m_stop_rendering = true;
				}
				if (event.window.event == SDL_WINDOWEVENT_RESTORED)
				{
					m_stop_rendering = false;
				}
				if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(m_window))
				{
					should_quit = true;
				}
			}
		}

		if (m_stop_rendering)
		{
			// slow loop
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		if (m_resize_requested)
		{
			resize_swapchain();
		}

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame();

		ImGui::NewFrame();

		if (ImGui::Begin("background"))
		{
			ImGui::SliderFloat("Render scale", &m_render_scale, 0.3f, 1.0f);
			ComputeEffect& selected = m_background_effects[m_current_background_effect];

			ImGui::Text("Selected effect: ", selected.name);
			ImGui::SliderInt("Effect index", &m_current_background_effect, 0, static_cast<u32>( m_background_effects.size() - 1 ));

			ImGui::InputFloat4("data[0]", std::bit_cast<float*>( &selected.data.data[0] ));
			ImGui::InputFloat4("data[1]", std::bit_cast<float*>( &selected.data.data[1] ));
			ImGui::InputFloat4("data[2]", std::bit_cast<float*>( &selected.data.data[2] ));
			ImGui::InputFloat4("data[3]", std::bit_cast<float*>( &selected.data.data[3] ));

		}
		ImGui::End();

		ImGui::Render();

		draw();
		// Update and Render additional Platform Windows
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

	}
}

VmaAllocator Engine::GetAllocator()
{
	Lud::assert::ne(m_allocator, VMA_NULL);
	return m_allocator;
}

vk::Device Engine::GetDevice()
{
	Lud::assert::eq(m_initialised);
	return m_device;
}

void Engine::init_vulkan()
{
	vkb::InstanceBuilder builder;

	vkb::Instance vkb_inst = builder.set_app_name("NES Emulator")
	#ifdef GRAPHICS_DEBUG
		.request_validation_layers(true)
		.use_default_debug_messenger()
	#endif // GRAPHICS_DEBUG
		.require_api_version(1, 3, 0)
		.build()
		.value();

	m_instance = vkb_inst.instance;
	m_debug_messenger = vkb_inst.debug_messenger;

	VkSurfaceKHR surface;
	SDL_Vulkan_CreateSurface(m_window, m_instance, &surface);

	m_surface = surface;

	// vulkan 1.3 features
	vk::PhysicalDeviceVulkan13Features features_13{};
	features_13.dynamicRendering = vk::True;
	features_13.synchronization2 = vk::True;

	// vulkan 1.2 features
	vk::PhysicalDeviceVulkan12Features features_12{};
	features_12.bufferDeviceAddress = vk::True;
	features_12.descriptorIndexing = vk::True;

	vkb::PhysicalDeviceSelector selector{ vkb_inst };
	vkb::PhysicalDevice vkb_physical_device = selector
		.set_minimum_version(1, 3)
		.add_required_extension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)
		.set_required_features_13(features_13)
		.set_required_features_12(features_12)
		.set_surface(m_surface)
		.select()
		.value();


	vkb::DeviceBuilder device_buidler{ vkb_physical_device };

	vkb::Device vkb_device = device_buidler.build().value();

	m_physical_device = vkb_physical_device.physical_device;
	m_device = vkb_device.device;

	m_graphics_queue.queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
	m_graphics_queue.family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

	m_present_queue.queue = vkb_device.get_queue(vkb::QueueType::present).value();
	m_present_queue.family = vkb_device.get_queue_index(vkb::QueueType::present).value();

	VmaAllocatorCreateInfo allocator_info = {};
	allocator_info.physicalDevice = m_physical_device;
	allocator_info.device = m_device;
	allocator_info.instance = m_instance;
	allocator_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

	VK_CHECK(vmaCreateAllocator(&allocator_info, &m_allocator));
	m_deletion_queue.PushFunction([&]()
		{
			vmaDestroyAllocator(m_allocator);
		});

}

void Engine::init_swapchain()
{
	create_swapchain(m_window_extent.width, m_window_extent.height);

	vk::Extent3D draw_image_extent(m_window_extent.width, m_window_extent.height, 1);

	m_draw_image.format = vk::Format::eR16G16B16A16Sfloat;
	m_draw_image.extent = draw_image_extent;

	vk::ImageUsageFlags draw_image_usage =
		vk::ImageUsageFlagBits::eTransferDst |
		vk::ImageUsageFlagBits::eTransferSrc |
		vk::ImageUsageFlagBits::eStorage |
		vk::ImageUsageFlagBits::eColorAttachment;

	VkImageCreateInfo draw_image_info = vkinit::image_create_info(m_draw_image.format, draw_image_usage, draw_image_extent);

	VmaAllocationCreateInfo draw_image_alloc_info = {};
	draw_image_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	draw_image_alloc_info.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	VK_CHECK(vmaCreateImage(
		m_allocator,
		&draw_image_info,
		&draw_image_alloc_info,
		std::bit_cast<VkImage*>( &m_draw_image.image ),
		&m_draw_image.allocation,
		nullptr
	));

	auto draw_image_view_info = vkinit::image_view_create_info(m_draw_image.format, m_draw_image.image, vk::ImageAspectFlagBits::eColor);

	m_draw_image.view = m_device.createImageView(draw_image_view_info);

	m_depth_image.format = vk::Format::eD32Sfloat;
	m_depth_image.extent = draw_image_extent;

	vk::ImageUsageFlags depth_image_usage =
		vk::ImageUsageFlagBits::eDepthStencilAttachment;

	VkImageCreateInfo depth_image_info = vkinit::image_create_info(m_depth_image.format, depth_image_usage, draw_image_extent);

	vmaCreateImage(m_allocator, &depth_image_info, &draw_image_alloc_info, std::bit_cast<VkImage*>( &m_depth_image.image ), &m_depth_image.allocation, nullptr);

	auto depth_image_view = vkinit::image_view_create_info(m_depth_image.format, m_depth_image.image, vk::ImageAspectFlagBits::eDepth);

	m_depth_image.view = m_device.createImageView(depth_image_view);

	m_deletion_queue.PushFunction([=]()
		{
			m_device.destroyImageView(m_draw_image.view);
			vmaDestroyImage(m_allocator, m_draw_image.image, m_draw_image.allocation);

			m_device.destroyImageView(m_depth_image.view);
			vmaDestroyImage(m_allocator, m_depth_image.image, m_depth_image.allocation);
		});
}


void Engine::init_commands()
{
	const auto cmd_pool_create_info = vkinit::command_pool_create_info(m_graphics_queue.family, vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

	for (size_t i = 0; i < FRAME_OVERLAP; i++)
	{
		m_frames[i].command_pool = m_device.createCommandPool(cmd_pool_create_info);

		const auto cmd_alloc_info = vkinit::command_buffer_allocate_info(m_frames[i].command_pool, 1);

		m_frames[i].command_buffer = m_device.allocateCommandBuffers(cmd_alloc_info).front();
	}

	m_imm_command_pool = m_device.createCommandPool(cmd_pool_create_info);
	vk::CommandBufferAllocateInfo cmd_alloc_info = vkinit::command_buffer_allocate_info(m_imm_command_pool, 1);

	m_imm_command_buffer = m_device.allocateCommandBuffers(cmd_alloc_info).front();

	m_deletion_queue.PushFunction([&]()
		{
			m_device.destroyCommandPool(m_imm_command_pool);
		});
}


void Engine::init_sync_structures()
{
	auto fence_info = vkinit::fence_create_info(vk::FenceCreateFlagBits::eSignaled);
	auto semaphore_info = vkinit::semaphore_create_info();
	for (size_t i = 0; i < FRAME_OVERLAP; i++)
	{
		m_frames[i].render_fence = m_device.createFence(fence_info);
		m_frames[i].render_semaphore = m_device.createSemaphore(semaphore_info);
		m_frames[i].swapchain_semaphore = m_device.createSemaphore(semaphore_info);
	}

	m_imm_fence = m_device.createFence(fence_info);
	m_deletion_queue.PushFunction([=]()
		{
			m_device.destroyFence(m_imm_fence);
		});
}

void Engine::init_descriptors()
{
	std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> sizes = {
		{vk::DescriptorType::eStorageImage,  1},
		{vk::DescriptorType::eUniformBuffer, 1}
	};
	m_descriptor_allocator.Init(m_device, 10, sizes);


	// make descriptor set layout for compute draw
	{
		auto builder = DescriptorLayoutBuilder();
		m_draw_image_descriptor_layout = builder
			.AddBinding(0, vk::DescriptorType::eStorageImage)
			.Build(m_device, vk::ShaderStageFlagBits::eCompute);
	}

	{
		auto builder = DescriptorLayoutBuilder();
		m_single_image_descriptor_layout = builder
			.AddBinding(0, vk::DescriptorType::eCombinedImageSampler)
			.Build(m_device, vk::ShaderStageFlagBits::eFragment);
	}

	{
		auto builder = DescriptorLayoutBuilder();
		m_scene_data_descriptor_layout = builder
			.AddBinding(0, vk::DescriptorType::eUniformBuffer)
			.Build(m_device, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);
	}


	m_draw_image_descriptors = m_descriptor_allocator.Allocate(m_device, m_draw_image_descriptor_layout);

	{
		auto writer = DescriptorWriter();
		writer
			.WriteImage(0, m_draw_image.view, nullptr, vk::ImageLayout::eGeneral, vk::DescriptorType::eStorageImage)
			.UpdateSet(m_device, m_draw_image_descriptors);
	}

	for (int i = 0; i < FRAME_OVERLAP; i++)
	{
		std::vector<DescriptorAllocatorGrowable::PoolSizeRatio> frame_sizes = {
			{vk::DescriptorType::eStorageImage,         3},
			{vk::DescriptorType::eStorageBuffer,        3},
			{vk::DescriptorType::eUniformBuffer,        3},
			{vk::DescriptorType::eCombinedImageSampler, 4}
		};

		m_frames[i].frame_descriptor = DescriptorAllocatorGrowable{};
		m_frames[i].frame_descriptor.Init(m_device, 1000, frame_sizes);

		m_deletion_queue.PushFunction([&, i]()
			{
				m_frames[i].frame_descriptor.DestroyPools(m_device);
			});
	}



	m_deletion_queue.PushFunction([&]()
		{
			m_descriptor_allocator.DestroyPools(m_device);
			m_device.destroyDescriptorSetLayout(m_draw_image_descriptor_layout);
			m_device.destroyDescriptorSetLayout(m_scene_data_descriptor_layout);
			m_device.destroyDescriptorSetLayout(m_single_image_descriptor_layout);
		});

}

void Engine::init_pipelines()
{
	init_background_pipeline();

	init_mesh_pipeline();
}

void Engine::init_background_pipeline()
{
	vk::PushConstantRange push_constants(vk::ShaderStageFlagBits::eCompute, 0, sizeof(ComputePushConstants<4>));
	vk::PipelineLayoutCreateInfo compute_layout({}, m_draw_image_descriptor_layout, push_constants);

	m_gradient_pipeline_layout = m_device.createPipelineLayout(compute_layout);

	auto gradient_shader = vkutil::load_shader_module("Shader/SPIRV/gradient_color.comp.spv", m_device);

	vk::PipelineShaderStageCreateInfo stage_info({}, vk::ShaderStageFlagBits::eCompute, gradient_shader, "main");

	vk::ComputePipelineCreateInfo compute_info({}, stage_info, m_gradient_pipeline_layout);

	ComputeEffect gradient;
	gradient.layout = m_gradient_pipeline_layout;
	gradient.name = "gradient";
	gradient.data = { {glm::vec4(1,0,0,1), glm::vec4(0,0,1,1)} };

	VK_CHECK(m_device.createComputePipelines(VK_NULL_HANDLE, 1, &compute_info, nullptr, &gradient.pipeline));


	auto sky_shader = vkutil::load_shader_module("Shader/SPIRV/sky.comp.spv", m_device);
	compute_info.stage.module = sky_shader;

	ComputeEffect sky;
	sky.layout = m_gradient_pipeline_layout;
	sky.name = "sky";
	sky.data = { {glm::vec4(0.1, 0.2, 0.4, 0.97)} };

	VK_CHECK(m_device.createComputePipelines(VK_NULL_HANDLE, 1, &compute_info, nullptr, &sky.pipeline));


	m_background_effects.push_back(gradient);
	m_background_effects.push_back(sky);

	m_device.destroyShaderModule(gradient_shader);
	m_device.destroyShaderModule(sky_shader);

	m_deletion_queue.PushFunction([&]()
		{
			m_device.destroyPipelineLayout(m_gradient_pipeline_layout);
			for (const auto& effect : m_background_effects)
			{
				m_device.destroyPipeline(effect.pipeline);
			}
		});
}

void Engine::init_mesh_pipeline()
{
	auto frag_module = vkutil::load_shader_module("Shader/SPIRV/tex_image.frag.spv", m_device);
	auto vert_module = vkutil::load_shader_module("Shader/SPIRV/colored_triangle_mesh.vert.spv", m_device);

	vk::PushConstantRange buffer_range;
	buffer_range.offset = 0;
	buffer_range.size = sizeof(GPUDrawPushConstants);
	buffer_range.stageFlags = vk::ShaderStageFlagBits::eVertex;

	auto pipeline_layout_info = vkinit::pipeline_layout_create_info();
	pipeline_layout_info.setPushConstantRanges(buffer_range);
	pipeline_layout_info.setSetLayouts(m_single_image_descriptor_layout);

	m_mesh_pipeline_layout = m_device.createPipelineLayout(pipeline_layout_info);

	m_mesh_pipeline = vkutil::PipelineBuilder()
		.SetPipelineLayout(m_mesh_pipeline_layout)
		.SetShaders(vert_module, frag_module)
		.SetInputTopology(vk::PrimitiveTopology::eTriangleList)
		.SetPolygonMode(vk::PolygonMode::eFill)
		.SetCullMode(vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise)
		.SetMultisamplingNone()
		//.EnableBlendingAdditive()
		.DisableBlending()
		.EnableDepthTest(true, vk::CompareOp::eGreaterOrEqual)
		.SetColorAttachmentFormat(m_draw_image.format)
		.SetDepthFormat(m_depth_image.format)
		.Build(m_device);

	m_device.destroyShaderModule(frag_module);
	m_device.destroyShaderModule(vert_module);

	m_deletion_queue.PushFunction([&]()
		{
			m_device.destroyPipelineLayout(m_mesh_pipeline_layout);
			m_device.destroyPipeline(m_mesh_pipeline);
		});
}

void Engine::init_default_data()
{
	m_test_meshes = load_gltf_meshes(this, "assets/basicmesh.glb").value();

	uint32_t white;
	uint32_t grey;
	uint32_t black;
	uint32_t magenta;

	if constexpr (std::endian::native == std::endian::little)
	{
		white   = std::byteswap(0xFFFFFFFF);
		grey    = std::byteswap(0xAAAAAAFF);
		black   = std::byteswap(0x000000FF);
		magenta = std::byteswap(0xFF00FFFF);
	}
	else
	{
		white   = 0xFFFFFFFF;
		grey    = 0xAAAAAAFF;
		black   = 0x000000FF;
		magenta = 0xFF00FFFF;
	}


	m_white_image = create_image(
		std::bit_cast<void*>( &white ), { 1, 1, 1 }, vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eSampled);
	m_grey_image = create_image(
		std::bit_cast<void*>( &grey ), { 1, 1, 1 }, vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eSampled);
	m_black_image = create_image(
		std::bit_cast<void*>( &black ), { 1, 1, 1 }, vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eSampled);

	std::array<uint32_t, 16 * 16> pixels;
	for (size_t x = 0; x < 16; x++)
	{
		for (size_t y = 0; y < 16; y++)
		{
			pixels[y * 16 + x] = ( ( x % 2 ) ^ ( y % 2 ) ) ? magenta : black;
		}
	}

	m_error_checkerboard_image = create_image(
		pixels.data(), { 16, 16, 1 }, vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eSampled);


	{
		vk::SamplerCreateInfo sampler_info;
		sampler_info.minFilter = vk::Filter::eNearest;
		sampler_info.magFilter = vk::Filter::eNearest;
		m_default_sampler_nearest = m_device.createSampler(sampler_info);
	}

	{
		vk::SamplerCreateInfo sampler_info;
		sampler_info.minFilter = vk::Filter::eLinear;
		sampler_info.magFilter = vk::Filter::eLinear;
		m_default_sampler_linear = m_device.createSampler(sampler_info);
	}

	m_deletion_queue.PushFunction([&]()
		{
			m_device.destroySampler(m_default_sampler_linear);
			m_device.destroySampler(m_default_sampler_nearest);
			destroy_image(m_white_image);
			destroy_image(m_black_image);
			destroy_image(m_grey_image);
			destroy_image(m_error_checkerboard_image);
		});

}

void Engine::create_swapchain(u32 width, u32 height)
{
	vkb::SwapchainBuilder swapchain_builder{ m_physical_device, m_device, m_surface };
	m_swapchain.format = vk::Format::eB8G8R8A8Unorm;

	vkb::Swapchain vkb_swapchain = swapchain_builder
		//.use_default_format_selection()
		.set_desired_format(vk::SurfaceFormatKHR(m_swapchain.format, vk::ColorSpaceKHR::eSrgbNonlinear))
		.set_desired_present_mode(static_cast<VkPresentModeKHR>( vk::PresentModeKHR::eFifo ))
		.set_desired_extent(width, height)
		.add_image_usage_flags(static_cast<VkImageUsageFlagBits>( vk::ImageUsageFlagBits::eTransferDst ))
		.build()
		.value();

	m_swapchain.extent = vkb_swapchain.extent;
	m_swapchain.swapchain = vkb_swapchain.swapchain;

	const auto images = vkb_swapchain.get_images().value();
	const auto views = vkb_swapchain.get_image_views().value();


	m_swapchain.frames.resize(images.size());

	for (size_t i = 0; i < images.size(); i++)
	{
		m_swapchain.frames[i].image = images[i];
		m_swapchain.frames[i].view = views[i];
	}


}

void Engine::destroy_swapchain()
{
	m_device.destroySwapchainKHR(m_swapchain);

	for (const auto& image_frames : m_swapchain.frames)
	{
		m_device.destroyImageView(image_frames.view);
	}
}

AllocatedBuffer Engine::create_buffer(size_t alloc_size, vk::BufferUsageFlags usage, VmaMemoryUsage memory_usage)
{
	VkBufferCreateInfo info = vkinit::buffer_create_info(alloc_size, usage);

	VmaAllocationCreateInfo vma_alloc_info = {};
	vma_alloc_info.usage = memory_usage;
	vma_alloc_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

	AllocatedBuffer buffer;


	VK_CHECK(vk::Result(vmaCreateBuffer(
		m_allocator,
		&info,
		&vma_alloc_info,
		std::bit_cast<VkBuffer*>( &buffer.buffer ),
		&buffer.allocation,
		&buffer.info
	)));

	return buffer;

}

void Engine::destroy_buffer(const AllocatedBuffer& buffer)
{
	vmaDestroyBuffer(m_allocator, buffer.buffer, buffer.allocation);
}

AllocatedImage Engine::create_image(vk::Extent3D size, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped)
{
	AllocatedImage image;
	image.format = format;
	image.extent = size;

	VkImageCreateInfo img_info = vkinit::image_create_info(format, usage, size);
	if (mipmapped)
	{
		img_info.mipLevels = static_cast<u32>( std::floor(std::log2(std::max(size.width, size.height))) ) + 1;
	}

	// allways allocate images on dedicated GPU memory
	VmaAllocationCreateInfo alloc_info = {};
	alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	alloc_info.requiredFlags = static_cast<VkMemoryPropertyFlags>( vk::MemoryPropertyFlagBits::eDeviceLocal );

	VK_CHECK(vmaCreateImage(m_allocator, &img_info, &alloc_info, std::bit_cast<VkImage*>( &image.image ), &image.allocation, nullptr));

	vk::ImageAspectFlags aspect_flags = vk::ImageAspectFlagBits::eColor;
	if (format == vk::Format::eD32Sfloat)
	{
		aspect_flags = vk::ImageAspectFlagBits::eDepth;
	}

	auto view_info = vkinit::image_view_create_info(format, image.image, aspect_flags);
	view_info.subresourceRange.levelCount = img_info.mipLevels;

	image.view = m_device.createImageView(view_info);

	return image;
}

AllocatedImage Engine::create_image(void* data, vk::Extent3D size, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped)
{
	size_t data_size = static_cast<size_t>( size.depth * size.width * size.height ) * 4;
	AllocatedBuffer upload_buffer = create_buffer(
		data_size,
		vk::BufferUsageFlagBits::eTransferSrc,
		VMA_MEMORY_USAGE_CPU_TO_GPU
	);

	memcpy(upload_buffer.info.pMappedData, data, data_size);

	AllocatedImage image = create_image(
		size,
		format,
		usage | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc,
		mipmapped
	);

	immediate_submit([&](vk::CommandBuffer cmd)
		{
			vkutil::transition_image(cmd, image.image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

			vk::BufferImageCopy copy_region;
			copy_region.bufferOffset = 0;
			copy_region.bufferRowLength = 0;
			copy_region.bufferImageHeight = 0;

			copy_region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			copy_region.imageSubresource.mipLevel = 0;
			copy_region.imageSubresource.baseArrayLayer = 0;
			copy_region.imageSubresource.layerCount = 1;

			copy_region.imageExtent = size;

			cmd.copyBufferToImage(upload_buffer.buffer, image.image, vk::ImageLayout::eTransferDstOptimal, copy_region);

			vkutil::transition_image(cmd, image.image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
		});

	destroy_buffer(upload_buffer);

	return image;
}

void Engine::destroy_image(const AllocatedImage& img)
{
	m_device.destroyImageView(img.view);
	vmaDestroyImage(m_allocator, img.image, img.allocation);
}

GPUMeshBuffers Engine::UploadMesh(std::span<u32> indices, std::span<Vertex> vertices)
{
	const size_t vertex_buffer_size = vertices.size_bytes();
	const size_t index_buffer_size = indices.size_bytes();

	GPUMeshBuffers surface;

	surface.vertex_buffer = create_buffer(
		vertex_buffer_size,
		vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eShaderDeviceAddress,
		VMA_MEMORY_USAGE_GPU_ONLY
	);

	vk::BufferDeviceAddressInfo device_address_info(surface.vertex_buffer.buffer);
	surface.address = m_device.getBufferAddress(device_address_info);

	surface.index_buffer = create_buffer(
		index_buffer_size,
		vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
		VMA_MEMORY_USAGE_GPU_ONLY
	);


	AllocatedBuffer staging = create_buffer(
		vertex_buffer_size + index_buffer_size,
		vk::BufferUsageFlagBits::eTransferSrc,
		VMA_MEMORY_USAGE_CPU_ONLY
	);

	void* data = staging.allocation->GetMappedData();

	// copy vertex buffer
	std::memcpy(data, vertices.data(), vertex_buffer_size);
	std::memcpy(std::bit_cast<char*>( data ) + vertex_buffer_size, indices.data(), index_buffer_size);

	immediate_submit([&](vk::CommandBuffer cmd)
		{
			vk::BufferCopy vertex_copy(0, 0, vertex_buffer_size);
			cmd.copyBuffer(staging.buffer, surface.vertex_buffer.buffer, vertex_copy);

			vk::BufferCopy index_copy(vertex_buffer_size, 0, index_buffer_size);
			cmd.copyBuffer(staging.buffer, surface.index_buffer.buffer, index_copy);
		});

	m_deletion_queue.PushFunction([=]()
		{
			destroy_buffer(surface.vertex_buffer);
			destroy_buffer(surface.index_buffer);
		});

	destroy_buffer(staging);

	return surface;

}

void Engine::init_imgui()
{
	vk::DescriptorPoolSize pool_sizes[] = {
		{vk::DescriptorType::eSampler              , 1000 },
		{vk::DescriptorType::eCombinedImageSampler , 1000 },
		{vk::DescriptorType::eSampledImage         , 1000 },
		{vk::DescriptorType::eStorageImage         , 1000 },
		{vk::DescriptorType::eUniformTexelBuffer   , 1000 },
		{vk::DescriptorType::eStorageTexelBuffer   , 1000 },
		{vk::DescriptorType::eUniformBuffer        , 1000 },
		{vk::DescriptorType::eStorageBuffer        , 1000 },
		{vk::DescriptorType::eUniformBufferDynamic , 1000 },
		{vk::DescriptorType::eStorageBufferDynamic , 1000 },
		{vk::DescriptorType::eInputAttachment      , 1000 },
	};


	vk::DescriptorPoolCreateInfo imgui_pool_info = vk::DescriptorPoolCreateInfo(
		vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		1000,
		static_cast<u32>( IM_ARRAYSIZE(pool_sizes) ),
		pool_sizes
	);
	auto imgui_pool = m_device.createDescriptorPool(imgui_pool_info);


	vk::PipelineRenderingCreateInfoKHR pipeline_create_info;
	pipeline_create_info.colorAttachmentCount = 1;
	pipeline_create_info.pColorAttachmentFormats = &m_swapchain.format;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	 // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplSDL2_InitForVulkan(m_window);

	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = m_instance;
	init_info.PhysicalDevice = m_physical_device;
	init_info.Device = m_device;
	init_info.Queue = m_graphics_queue.queue;
	init_info.QueueFamily = m_graphics_queue.family;
	init_info.DescriptorPool = imgui_pool;
	init_info.MinImageCount = 3;
	init_info.ImageCount = 3;
	init_info.UseDynamicRendering = true;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	init_info.PipelineRenderingCreateInfo = pipeline_create_info;

	ImGui_ImplVulkan_Init(&init_info);

	ImFontConfig fontConfig;
	fontConfig.FontDataOwnedByAtlas = false;
	io.Fonts->AddFontDefault();
	ImFont* main_font = io.Fonts->AddFontFromMemoryCompressedTTF(OpenSans_compressed_data, OpenSans_compressed_size, 20.0f, &fontConfig);
	io.FontDefault = main_font;


	immediate_submit([&](vk::CommandBuffer cmd)
		{
			ImGui_ImplVulkan_CreateFontsTexture();
		});

	m_deletion_queue.PushFunction([=]()
		{
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplSDL2_Shutdown();
			ImGui::DestroyContext();
			m_device.destroyDescriptorPool(imgui_pool);
		});



}

void Engine::immediate_submit(std::function<void(vk::CommandBuffer cmd)>&& function)
{
	m_device.resetFences(m_imm_fence);
	m_imm_command_buffer.reset();

	vk::CommandBuffer cmd = m_imm_command_buffer;

	auto cmd_begin_info = vkinit::command_buffer_begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

	cmd.begin(cmd_begin_info);

	function(cmd);

	cmd.end();

	auto cmd_info = vkinit::command_buffer_submit_info(cmd);
	auto submit = vkinit::submit_info(&cmd_info, nullptr, nullptr);

	m_graphics_queue.queue.submit2(submit, m_imm_fence);

	VK_CHECK(m_device.waitForFences(m_imm_fence, vk::True, std::numeric_limits<uint64_t>::max()));

}

void Engine::draw_imgui(vk::CommandBuffer cmd, vk::ImageView view)
{
	auto color_attachment = vkinit::attachment_info(view, nullptr, vk::ImageLayout::eGeneral);
	auto render_info = vkinit::rendering_info(m_swapchain.extent, &color_attachment, nullptr);

	cmd.beginRendering(render_info);

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

	cmd.endRendering();
}


void Engine::cleanup()
{
	loaded_engine = nullptr;
	if (!m_initialised) return;
	// wait for GPU to stop
	m_device.waitIdle();


	for (auto& frame : m_frames)
	{
		m_device.destroyCommandPool(frame.command_pool);
		m_device.destroyFence(frame.render_fence);
		m_device.destroySemaphore(frame.render_semaphore);
		m_device.destroySemaphore(frame.swapchain_semaphore);

		frame.deletion_queue.Flush();
	}
	m_deletion_queue.Flush();

	destroy_swapchain();
	m_device.destroy();
	m_instance.destroySurfaceKHR(m_surface);
	vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
	m_instance.destroy();

	SDL_DestroyWindow(m_window);

}

void DeletionQueue::PushFunction(std::function<void()>&& function)
{
	deletors.push_back(function);
}

void DeletionQueue::Flush()
{
	for (auto it = deletors.rbegin(); it != deletors.rend(); ++it)
	{
		( *it )( );
	}

	deletors.clear();
}


}
