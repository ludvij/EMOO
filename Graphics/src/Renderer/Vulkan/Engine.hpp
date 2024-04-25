#ifndef GRAPHICS_ENGINE_HEADER
#define GRAPHICS_ENGINE_HEADER

#include "Core.hpp"

#include "vkutil/Images.hpp"
#include "vkutil/Pipelines.hpp"
#include "vkutil/Types.hpp"


#include "Descriptors.hpp"

#include "Renderer/RendererAPI.hpp"


struct SDL_Window;

namespace Ui
{

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
	DescriptorAllocatorGrowable frame_descriptor;
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
	Engine();
	~Engine();

	void Run();

	VmaAllocator GetAllocator();
	vk::Device GetDevice();

	void CreateSquare(const Rect& bounds);




private:
	void init();
	void cleanup();

	void draw();

	void draw_background(vk::CommandBuffer cmd);

	void draw_geometry(vk::CommandBuffer cmd);

	void resize_swapchain();

	Detail::GPUMeshBuffers upload_mesh(std::span<u32> indices, std::span<Detail::Vertex> vertices);

	void build_emulator_screen();
	void resize_emulator_screen();
	Rect get_emulator_screen_bounds(int w, int h) const;

private:

	bool m_initialised{ false };
	u32 m_frame_number{ 0 };
	bool m_stop_rendering{ false };
	vk::Extent2D m_window_extent{ 1700, 900 };

	SDL_Window* m_window{ nullptr };


	bool m_resize_requested{ false };


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

	Detail::AllocatedBuffer create_buffer(size_t alloc_size, vk::BufferUsageFlags usage, VmaMemoryUsage memory_usage);
	void destroy_buffer(const Detail::AllocatedBuffer& buffer);

	Detail::AllocatedImage create_image(vk::Extent3D size, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped = false);
	Detail::AllocatedImage create_image(void* data, vk::Extent3D size, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped = false);
	void destroy_image(const Detail::AllocatedImage& img);


private:
	// vulkan structures
	vk::Instance               m_instance;
	vk::PhysicalDevice         m_physical_device;
	vk::Device                 m_device;
	vk::SurfaceKHR             m_surface;
	vkutil::SwapchainBundle    m_swapchain;
	vk::DebugUtilsMessengerEXT m_debug_messenger;


	Detail::FrameData m_frames[FRAME_OVERLAP];

	// queue
	vkutil::QueueBundle m_graphics_queue;
	vkutil::QueueBundle m_present_queue;

	// make deletions easier
	Detail::DeletionQueue m_deletion_queue;

	// memory allocator
	VmaAllocator m_allocator{ nullptr };

	// image to send to swapchain
	Detail::AllocatedImage m_draw_image;
	Detail::AllocatedImage m_depth_image;
	vk::Extent2D           m_draw_extent;
	float m_render_scale = 1.0f;

	// descriptors for compute shaders
	Detail::DescriptorAllocatorGrowable m_descriptor_allocator;
	vk::DescriptorSetLayout     m_single_image_descriptor_layout;


	Detail::GPUMeshBuffers m_rectangle;



	vk::PipelineLayout m_mesh_pipeline_layout;
	vk::Pipeline       m_mesh_pipeline;


	Detail::AllocatedImage m_error_checkerboard_image;

	vk::Sampler m_default_sampler_nearest;

	Detail::SceneData m_scene_data;

private:

	void init_imgui();

	void immediate_submit(std::function<void(vk::CommandBuffer cmd)>&& function);

	void draw_imgui(vk::CommandBuffer cmd, vk::ImageView view);


private:
	// immediate submit structures
	vk::Fence m_imm_fence;
	vk::CommandBuffer m_imm_command_buffer;
	vk::CommandPool   m_imm_command_pool;

private:

	//std::deque<Quad> m_quad_list;

};

}

#endif
