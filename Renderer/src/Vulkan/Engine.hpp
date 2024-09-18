#ifndef GRAPHICS_ENGINE_HEADER
#define GRAPHICS_ENGINE_HEADER

#include "BatchRenderer.hpp"
#include "Core.hpp"
#include "RendererAPI.hpp"
#include "vkutil/Descriptors.hpp"
#include "vkutil/Images.hpp"
#include "vkutil/Pipelines.hpp"
#include "vkutil/Types.hpp"


struct SDL_Window;

namespace Renderer
{

class VulkanTexture;
namespace Detail
{

class DeletionQueue
{
public:

	void PushFunction(std::function<void()>&& function);

	void Flush();


private:
	std::deque<std::function<void()>> deletors;
};

struct FrameData
{
	vk::CommandPool   command_pool;
	vk::CommandBuffer command_buffer;

	vk::Semaphore swapchain_semaphore;
	vk::Semaphore render_semaphore;

	vk::Fence render_fence;

	DeletionQueue deletion_queue;
	vkutil::DescriptorAllocatorGrowable frame_descriptor;
};

struct SceneData
{
	glm::mat4 view;
	glm::mat4 proj;
};
}


constexpr u32 FRAME_OVERLAP = 2;

class Engine
{
public:

	static Engine& Get();
	~Engine();

	void Init(std::shared_ptr<Window::IWindow> window, bool use_imgui=true);
	void Cleanup();

	void Draw();
	void Resize();
	void RequestResize();

	void AddTextureToBatcher(VulkanBindlessTexture* texture);
	void RemoveTextureFromBatcher(VulkanBindlessTexture* texture);


	Detail::AllocatedImage CreateImage(vk::Extent3D size, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped = false);
	Detail::AllocatedImage CreateImage(void* data, vk::Extent3D size, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped = false);
	void DestroyImage(const Detail::AllocatedImage& img);
	void SetImageData(Detail::AllocatedImage image, void* data);

	Detail::AllocatedBuffer CreateBuffer(size_t alloc_size, vk::BufferUsageFlags usage, VmaMemoryUsage memory_usage) const;
	void DestroyBuffer(const Detail::AllocatedBuffer& buffer) const;


	void SubmitDrawRect(std::span<Detail::Vertex> vertices);


	vk::Device GetDevice() const
	{
		return m_device;
	}

	VmaAllocator GetAllocator() const
	{
		return m_allocator;
	}

	vk::Sampler GetSampler() const
	{
		return m_default_sampler_nearest;
	}



private:
	Engine() = default;


	void draw_background(vk::CommandBuffer cmd) const;

	void draw_geometry(vk::CommandBuffer cmd);


	Detail::GPUMeshBuffers upload_mesh(std::span<u32> indices, std::span<Detail::Vertex> vertices);


private:

	bool m_initialised{ false };
	u32 m_frame_number{ 0 };
	vk::Extent2D m_window_extent;

	std::shared_ptr<Window::IWindow> m_window{ nullptr };

private:

	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_sync_structures();
	void init_descriptors();

	void init_pipelines();

	void init_mesh_pipeline();

	void init_default_data();


	void create_swapchain(u32 width, u32 height);
	void destroy_swapchain();

	Detail::FrameData& get_current_frame()
	{
		return m_frames[m_frame_number % FRAME_OVERLAP];
	}





private:
	// vulkan structures
	vk::Instance               m_instance;
	vk::PhysicalDevice         m_physical_device;
	vk::Device                 m_device;
	vk::SurfaceKHR             m_surface;
	vkutil::SwapchainBundle    m_swapchain;
	vk::DebugUtilsMessengerEXT m_debug_messenger;

	bool m_resize_requested = false;


	Detail::FrameData m_frames[FRAME_OVERLAP];

	// queue
	vkutil::QueueBundle m_graphics_queue;

	// make deletions easier
	Detail::DeletionQueue m_deletion_queue;

	// memory allocator
	VmaAllocator m_allocator{ nullptr };

	// image to send to swapchain
	Detail::AllocatedImage m_draw_image;
	Detail::AllocatedImage m_depth_image;
	vk::Extent2D           m_draw_extent;

	vk::DescriptorSetLayout m_bindless_image_descriptor_layout;


	vk::PipelineLayout m_mesh_pipeline_layout;
	vk::Pipeline       m_mesh_pipeline;

	vk::Sampler m_default_sampler_nearest;

	Detail::SceneData m_scene_data{};

public:
	void ImmediateSubmit(std::function<void(vk::CommandBuffer cmd)>&& function);

private:

	void init_imgui();


	void draw_imgui(vk::CommandBuffer cmd, vk::ImageView view) const;


private:
	// immediate submit structures
	vk::Fence m_imm_fence;
	vk::CommandBuffer m_imm_command_buffer;
	vk::CommandPool   m_imm_command_pool;

	bool m_use_imgui = true;

private:

	BatchRenderer* m_batcher{ nullptr };
	friend class BatchRenderer;
	friend class VulkanTexture;
};

}

#endif
