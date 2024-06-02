#include "Engine.hpp"

#include <chrono>
#include <thread>

#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

#include "ImGui/CascadiaMono-Regular.embed"
#include "ImGui/OpenSans-Regular.embed"

#include <glm/gtx/transform.hpp>

#include <lud_assert.hpp>

#include <print>

#include <vkBootstrap.h>
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>


// Shader code
extern const size_t sg_embed_mesh_frag_size;
extern const uint32_t sg_embed_mesh_frag[];

extern const size_t sg_embed_mesh_vert_size;
extern const uint32_t sg_embed_mesh_vert[];


#include "vkutil/Descriptors.hpp"
#include "vkutil/Initializers.hpp"
#include "Window/SDL/SDLWindow.hpp"

namespace Ui
{

constexpr uint32_t UNIFORM_BINDING = 0;
constexpr uint32_t SAMPLER_BINDING = 1;

using namespace Detail;

Engine& Engine::Get()
{
	static Engine s_instance;
	return s_instance;
}

Engine::~Engine()
{
	Cleanup();
}

void Engine::Init(std::shared_ptr<IWindow> window, bool use_imgui)
{
	m_use_imgui = use_imgui;
	m_window = window;
	const auto [w, h] = window->GetDimensions();
	m_window_extent.width = w;
	m_window_extent.height = h;
	init_vulkan();

	init_swapchain();

	init_commands();

	init_sync_structures();

	init_descriptors();

	init_pipelines();

	if (use_imgui)
	{
		init_imgui();
	}

	init_default_data();

	m_batcher = new BatchRenderer();

	m_initialised = true;
}

void Engine::Draw()
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

	m_draw_extent.width = static_cast<u32>( std::min(m_swapchain.extent.width, m_draw_image.extent.width) );
	m_draw_extent.height = static_cast<u32>( std::min(m_swapchain.extent.height, m_draw_image.extent.height) );

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
	if (m_use_imgui)
	{
		draw_imgui(cmd, m_swapchain.frames[swapchain_image_index].view);
	}

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

void Engine::draw_background(vk::CommandBuffer cmd) const
{
	constexpr vk::ImageSubresourceRange image_range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);

	vk::ClearColorValue color(40.0f / 255.0f, 44.0f / 255.0f, 52.0f / 255.0f, 0.0f);
	cmd.clearColorImage(m_draw_image.image, vk::ImageLayout::eGeneral, color, image_range);
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

	AllocatedBuffer scene_buffer = CreateBuffer(sizeof m_scene_data, vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU);
	vmaCopyMemoryToAllocation(m_allocator, &m_scene_data, scene_buffer.allocation, 0, sizeof m_scene_data);

	vk::DescriptorSet image_set = get_current_frame().frame_descriptor.Allocate(m_device, m_bindless_image_descriptor_layout);

	vkutil::DescriptorWriter writer;
	writer.WriteBuffer(UNIFORM_BINDING, scene_buffer.buffer, sizeof m_scene_data, 0, vk::DescriptorType::eUniformBuffer);
	m_batcher->PrepareDescriptor(SAMPLER_BINDING, writer);
	writer.UpdateSet(m_device, image_set);

	cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_mesh_pipeline_layout, 0, image_set, {});


	GPUDrawPushConstants push_constants{};
	push_constants.world_matrix = glm::mat4{ 1.0f };
	push_constants.vertex_buffer = m_batcher->GetAddress();

	cmd.pushConstants(m_mesh_pipeline_layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof GPUDrawPushConstants, &push_constants);

	m_batcher->Draw(cmd);


	cmd.endRendering();

	get_current_frame().deletion_queue.PushFunction([=, this]()
		{
			DestroyBuffer(scene_buffer);
		});
}

void Engine::Resize()
{
	if (!m_resize_requested)
	{
		return;
	}
	m_device.waitIdle();

	destroy_swapchain();

	auto [w, h] = m_window->GetDimensions();
	m_window_extent.width = w;
	m_window_extent.height = h;

	create_swapchain(m_window_extent.width, m_window_extent.height);

	m_scene_data.proj = glm::ortho(0.0f, static_cast<float>( w ), static_cast<float>( h ), 0.0f, -1.0f, 1.0f);

	m_resize_requested = false;
}

void Engine::RequestResize()
{
	m_resize_requested = true;
}

void Engine::AddTextureToBatcher(VulkanBindlessTexture* texture)
{
	m_batcher->AddTexture(texture);
}

void Engine::RemoveTextureFromBatcher(VulkanBindlessTexture* texture)
{
	m_batcher->RemoveTexture(texture);
}

void Engine::init_vulkan()
{
	vkb::InstanceBuilder builder;
	vkb::Instance vkb_inst = builder.set_app_name("NES Emulator")
	#ifdef GRAPHICS_DEBUG
		.request_validation_layers(true)
		.use_default_debug_messenger()
	#endif // GRAPHICS_DEBUG
	#if defined (GRAPHICS_DEBUG) || defined(GRAPHICS_SHOW_FPS)
		.enable_layer("VK_LAYER_LUNARG_monitor")
	#endif
		.require_api_version(1, 3, 0)
		.build()
		.value();

	m_instance = vkb_inst.instance;
	m_debug_messenger = vkb_inst.debug_messenger;


	m_surface = m_window->CreateVulkanSurface(m_instance);

	// vulkan 1.3 features
	auto features_13 = vk::PhysicalDeviceVulkan13Features()
		.setDynamicRendering(vk::True)
		.setSynchronization2(vk::True);
	// vulkan 1.2 features
	auto features_12 = vk::PhysicalDeviceVulkan12Features()
		.setDescriptorIndexing(vk::True)
		.setBufferDeviceAddress(vk::True)
		// non sized array
		.setRuntimeDescriptorArray(vk::True)
		// non bound descriptor sets
		.setDescriptorBindingPartiallyBound(vk::True)
		// STORAGE BUFFERS
		//.setDescriptorBindingStorageBufferUpdateAfterBind(vk::True)
		//.setShaderStorageBufferArrayNonUniformIndexing(vk::True)
		// SAMPLED IMAGES
		.setShaderSampledImageArrayNonUniformIndexing(vk::True)
		.setDescriptorBindingSampledImageUpdateAfterBind(vk::True)
		// STORAGE IMAGES
		//.setDescriptorBindingStorageImageUpdateAfterBind(vk::True)
		//.setShaderStorageImageArrayNonUniformIndexing(vk::True)
		;
	vk::PhysicalDeviceFeatures features{};


	vkb::PhysicalDeviceSelector selector{ vkb_inst };
	vkb::PhysicalDevice vkb_physical_device = selector
		.set_minimum_version(1, 3)
		.add_required_extension(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME)
		.set_required_features_13(features_13)
		.set_required_features_12(features_12)
		.set_required_features(features)
		.set_surface(m_surface)
		.select()
		.value();


	vkb::DeviceBuilder device_buidler{ vkb_physical_device };

	vkb::Device vkb_device = device_buidler.build().value();


	m_physical_device = vkb_physical_device.physical_device;
	m_device = vkb_device.device;


	m_graphics_queue.queue = vkb_device.get_queue(vkb::QueueType::graphics).value();
	m_graphics_queue.family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();


	VmaAllocatorCreateInfo allocator_info = {};
	allocator_info.physicalDevice = m_physical_device;
	allocator_info.device = m_device;
	allocator_info.instance = m_instance;
	allocator_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

	VK_CHECK(vmaCreateAllocator(&allocator_info, &m_allocator));
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
	[[maybe_unused]]
	const uint32_t sampler_count = m_physical_device.getProperties().limits.maxDescriptorSetSampledImages;

	m_bindless_image_descriptor_layout = vkutil::DescriptorLayoutBuilder()
		.SetBindless(vk::DescriptorType::eCombinedImageSampler)
		.AddBinding(UNIFORM_BINDING, vk::DescriptorType::eUniformBuffer, 1)
		.AddBinding(SAMPLER_BINDING, vk::DescriptorType::eCombinedImageSampler, 32)
		.Build(m_device, vk::ShaderStageFlagBits::eAll);


	for (int i = 0; i < FRAME_OVERLAP; i++)
	{
		std::vector<vkutil::DescriptorAllocatorGrowable::PoolSizeRatio> frame_sizes = {
			{vk::DescriptorType::eUniformBuffer,        1 },
			{vk::DescriptorType::eCombinedImageSampler, 32}
		};

		m_frames[i].frame_descriptor = vkutil::DescriptorAllocatorGrowable{};
		m_frames[i].frame_descriptor.Init(m_device, 2, frame_sizes);

		m_deletion_queue.PushFunction([&, i]()
			{
				m_frames[i].frame_descriptor.DestroyPools(m_device);
			});
	}



	m_deletion_queue.PushFunction([&]()
		{
			m_device.destroyDescriptorSetLayout(m_bindless_image_descriptor_layout);
		});

}

void Engine::init_pipelines()
{
	init_mesh_pipeline();
}


void Engine::init_mesh_pipeline()
{
	auto frag_module = vkutil::load_shader_module(sg_embed_mesh_frag, sg_embed_mesh_frag_size, m_device);
	auto vert_module = vkutil::load_shader_module(sg_embed_mesh_vert, sg_embed_mesh_vert_size, m_device);

	auto buffer_range = vk::PushConstantRange()
		.setOffset(0)
		.setSize(sizeof GPUDrawPushConstants)
		.setStageFlags(vk::ShaderStageFlagBits::eVertex);

	vk::PipelineLayoutCreateInfo pipeline_layout_info = vkinit::pipeline_layout_create_info()
		.setPushConstantRanges(buffer_range)
		.setSetLayouts(m_bindless_image_descriptor_layout);

	m_mesh_pipeline_layout = m_device.createPipelineLayout(pipeline_layout_info);

	m_mesh_pipeline = vkutil::PipelineBuilder()
		.SetPipelineLayout(m_mesh_pipeline_layout)
		.SetShaders(vert_module, frag_module)
		.SetInputTopology(vk::PrimitiveTopology::eTriangleList)
		.SetPolygonMode(vk::PolygonMode::eFill)
		.SetCullMode(vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise)
		.SetMultisamplingNone()
		.EnableBlendingAlphablend()
		//.DisableBlending()
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
	auto sampler_info = vk::SamplerCreateInfo()
		.setMinFilter(vk::Filter::eNearest)
		.setMagFilter(vk::Filter::eNearest)
		.setMipmapMode(vk::SamplerMipmapMode::eLinear);

	m_default_sampler_nearest = m_device.createSampler(sampler_info);

	const float w = static_cast<float>( m_window_extent.width );
	const float h = static_cast<float>( m_window_extent.height );

	m_scene_data.proj = glm::ortho(0.0f, w, h, 0.0f, -1.0f, 1.0f);
	m_scene_data.view = glm::mat4{ 1.0f };
	m_scene_data.view[1][1] *= -1;

	m_deletion_queue.PushFunction([&]()
		{
			m_device.destroySampler(m_default_sampler_nearest);
		});

}

void Engine::create_swapchain(u32 width, u32 height)
{
	vkb::SwapchainBuilder swapchain_builder{ m_physical_device, m_device, m_surface };
	m_swapchain.format = vk::Format::eB8G8R8A8Unorm;

	vkb::Swapchain vkb_swapchain = swapchain_builder
		//.use_default_format_selection()
		.set_desired_format(vk::SurfaceFormatKHR(m_swapchain.format, vk::ColorSpaceKHR::eSrgbNonlinear))
		.set_desired_present_mode(static_cast<VkPresentModeKHR>( vk::PresentModeKHR::eImmediate ))
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

AllocatedBuffer Engine::CreateBuffer(size_t alloc_size, vk::BufferUsageFlags usage, VmaMemoryUsage memory_usage)
{
	VkBufferCreateInfo info = vkinit::buffer_create_info(alloc_size, usage);

	VmaAllocationCreateInfo vma_alloc_info = {};
	vma_alloc_info.usage = memory_usage;
	vma_alloc_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

	AllocatedBuffer buffer;


	VK_CHECK(vmaCreateBuffer(
		m_allocator,
		&info,
		&vma_alloc_info,
		std::bit_cast<VkBuffer*>( &buffer.buffer ),
		&buffer.allocation,
		&buffer.info
	));

	return buffer;

}

void Engine::DestroyBuffer(const AllocatedBuffer& buffer)
{
	vmaDestroyBuffer(m_allocator, buffer.buffer, buffer.allocation);
}


AllocatedImage Engine::CreateImage(vk::Extent3D size, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped)
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
	alloc_info.requiredFlags = static_cast<VkMemoryPropertyFlags>(
		vk::MemoryPropertyFlagBits::eDeviceLocal
		);

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

AllocatedImage Engine::CreateImage(void* data, vk::Extent3D size, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped)
{
	size_t data_size = static_cast<size_t>( size.depth * size.width * size.height ) * 4;
	AllocatedBuffer upload_buffer = CreateBuffer(
		data_size,
		vk::BufferUsageFlagBits::eTransferSrc,
		VMA_MEMORY_USAGE_CPU_TO_GPU
	);

	memcpy(upload_buffer.info.pMappedData, data, data_size);

	AllocatedImage image = CreateImage(
		size,
		format,
		usage | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc,
		mipmapped
	);

	ImmediateSubmit([&](vk::CommandBuffer cmd)
		{
			vkutil::transition_image(cmd, image.image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

			vk::BufferImageCopy copy_region = vk::BufferImageCopy()
				.setBufferOffset(0)
				.setBufferRowLength(0)
				.setBufferImageHeight(0)
				.setImageSubresource({ vk::ImageAspectFlagBits::eColor, 0, 0, 1 })
				.setImageExtent(size);

			cmd.copyBufferToImage(upload_buffer.buffer, image.image, vk::ImageLayout::eTransferDstOptimal, copy_region);

			vkutil::transition_image(cmd, image.image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
		});

	DestroyBuffer(upload_buffer);

	return image;
}


void Engine::DestroyImage(const AllocatedImage& img)
{
	m_device.destroyImageView(img.view);
	vmaDestroyImage(m_allocator, img.image, img.allocation);
}

void Engine::SetImageData(Detail::AllocatedImage image, void* data)
{
	size_t data_size = static_cast<size_t>( image.extent.depth * image.extent.width * image.extent.height ) * sizeof u32;
	AllocatedBuffer upload_buffer = CreateBuffer(
		data_size,
		vk::BufferUsageFlagBits::eTransferSrc,
		VMA_MEMORY_USAGE_CPU_TO_GPU
	);

	vmaCopyMemoryToAllocation(m_allocator, data, upload_buffer.allocation, 0, data_size);

	ImmediateSubmit([&](vk::CommandBuffer cmd)
		{
			vkutil::transition_image(cmd, image.image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

			vk::BufferImageCopy copy_region = vk::BufferImageCopy()
				.setBufferOffset(0)
				.setBufferRowLength(0)
				.setBufferImageHeight(0)
				.setImageSubresource({ vk::ImageAspectFlagBits::eColor, 0, 0, 1 })
				.setImageExtent(image.extent);

			cmd.copyBufferToImage(upload_buffer.buffer, image.image, vk::ImageLayout::eTransferDstOptimal, copy_region);

			vkutil::transition_image(cmd, image.image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
		});

	DestroyBuffer(upload_buffer);
}


void Engine::SubmitDrawRect(std::span<Vertex> vertices)
{
	m_batcher->Add(vertices);
}

GPUMeshBuffers Engine::upload_mesh(std::span<u32> indices, std::span<Vertex> vertices)
{
	const size_t vertex_buffer_size = vertices.size_bytes();
	const size_t index_buffer_size = indices.size_bytes();

	GPUMeshBuffers surface;

	surface.vertex_buffer = CreateBuffer(
		vertex_buffer_size,
		vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eShaderDeviceAddress,
		VMA_MEMORY_USAGE_GPU_ONLY
	);

	vk::BufferDeviceAddressInfo device_address_info(surface.vertex_buffer.buffer);
	surface.address = m_device.getBufferAddress(device_address_info);

	surface.index_buffer = CreateBuffer(
		index_buffer_size,
		vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
		VMA_MEMORY_USAGE_GPU_ONLY
	);


	AllocatedBuffer staging = CreateBuffer(
		vertex_buffer_size + index_buffer_size,
		vk::BufferUsageFlagBits::eTransferSrc,
		VMA_MEMORY_USAGE_CPU_TO_GPU
	);


	vmaCopyMemoryToAllocation(m_allocator, vertices.data(), staging.allocation, 0, vertex_buffer_size);
	vmaCopyMemoryToAllocation(m_allocator, indices.data(), staging.allocation, vertex_buffer_size, index_buffer_size);
	// copy vertex buffer

	ImmediateSubmit([&](vk::CommandBuffer cmd)
		{
			vk::BufferCopy vertex_copy(0, 0, vertex_buffer_size);
			cmd.copyBuffer(staging.buffer, surface.vertex_buffer.buffer, vertex_copy);

			vk::BufferCopy index_copy(vertex_buffer_size, 0, index_buffer_size);
			cmd.copyBuffer(staging.buffer, surface.index_buffer.buffer, index_copy);
		});

	m_deletion_queue.PushFunction([=]()
		{
			DestroyBuffer(surface.vertex_buffer);
			DestroyBuffer(surface.index_buffer);
		});

	DestroyBuffer(staging);

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
	m_window->InitImguiForVulkan();

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
	fontConfig.FontDataOwnedByAtlas = true;
	io.Fonts->AddFontDefault();
	ImFont* main_font = io.Fonts->AddFontFromMemoryCompressedTTF(OpenSans_compressed_data, OpenSans_compressed_size, 20.0f, &fontConfig);
	m_monospace_font = io.Fonts->AddFontFromMemoryCompressedTTF(CascadiaMono_compressed_data, CascadiaMono_compressed_size, 20.0f, &fontConfig);
	io.FontDefault = main_font;
	io.Fonts->Build();


	ImmediateSubmit([&](vk::CommandBuffer cmd)
		{
			ImGui_ImplVulkan_CreateFontsTexture();
		});

	m_deletion_queue.PushFunction([=]()
		{
			ImGui_ImplVulkan_Shutdown();
			m_window->ShutdownImGuiWindow();
			ImGui::DestroyContext();
			m_device.destroyDescriptorPool(imgui_pool);
		});



}

void Engine::ImmediateSubmit(std::function<void(vk::CommandBuffer cmd)>&& function)
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

void Engine::Cleanup()
{
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
	delete m_batcher;
	vmaDestroyAllocator(m_allocator);
	destroy_swapchain();
	m_device.destroy();
	m_instance.destroySurfaceKHR(m_surface);
	vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
	m_instance.destroy();


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
