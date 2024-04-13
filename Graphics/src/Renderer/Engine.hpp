#ifndef GRAPHICS_ENGINE_HEADER
#define GRAPHICS_ENGINE_HEADER

#include "Vulkan/Core.hpp"

#include "Vulkan/vkutil/Swapchain.hpp"
#include "Vulkan/vkutil/Queue.hpp"
#include "Vulkan/vkutil/Images.hpp"
#include "Vulkan/vkutil/Pipelines.hpp"


#include "Vulkan/Descriptors.hpp"


struct SDL_Window;

namespace Ui
{

namespace Detail {

struct DeletionQueue
{
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& function);

	void flush();
};

struct AllocatedImage
{
	vk::Image     image;
	vk::ImageView view;
	vk::Format    format;
	vk::Extent3D  extent;
	VmaAllocation allocation;
};

}

struct FrameData 
{
	vk::CommandPool command_pool;
	vk::CommandBuffer command_buffer;

	vk::Semaphore swapchain_semaphore;
	vk::Semaphore render_semaphore;

	vk::Fence render_fence;

	Detail::DeletionQueue deletion_queue;
};

constexpr uint32_t FRAME_OVERLAP = 2;

class Engine
{
public:
	static Engine& Get();
	Engine();
	~Engine();

	void Run();

	VmaAllocator GetAllocator();
	vk::Device GetDevice();

private:
	void init();
	void cleanup();

	void draw();

	void draw_background(vk::CommandBuffer cmd);


private:

	bool m_initialised{ false };
	uint32_t m_frame_number{ 0 };
	bool m_stop_rendering{ false };
	vk::Extent2D m_window_extent{ 1700, 900 };

	SDL_Window* m_window{ nullptr };

private:

	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_sync_structures();
	void init_descriptors();

	void init_pipelines();
	void init_background_pipelines();


	void create_swapchain(uint32_t width, uint32_t height);
	void destroy_swapchain();

	FrameData& get_current_frame() { return m_frames[m_frame_number % FRAME_OVERLAP]; }

private: 
	// vulkan structures
	vk::Instance               m_instance;
	vk::PhysicalDevice         m_physical_device;
	vk::Device                 m_device;
	vk::SurfaceKHR             m_surface;
	vkutil::SwapchainBundle    m_swapchain;
	vk::DebugUtilsMessengerEXT m_debug_messenger;

	FrameData m_frames[FRAME_OVERLAP];

	// queue
	vkutil::QueueBundle m_graphics_queue;
	vkutil::QueueBundle m_present_queue;

	// make deletions easier
	Detail::DeletionQueue m_deletion_queue;

	// memory allocator
	VmaAllocator m_allocator{ nullptr };

	// image to send to swapchain
	Detail::AllocatedImage m_draw_image;
	vk::Extent2D           m_draw_extent;

	// descriptors for compute shaders
	Detail::DescriptorAllocator m_descriptor_allocator;
	vk::DescriptorSet           m_draw_image_descriptors;
	vk::DescriptorSetLayout     m_draw_image_descriptor_layout;

	vk::Pipeline       m_gradient_pipeline;
	vk::PipelineLayout m_gradient_pipeline_layout;

private:

	void init_imgui();

	void immediate_submit(std::function<void(vk::CommandBuffer cmd)>&& function);

	void draw_imgui(vk::CommandBuffer cmd, vk::ImageView view);


private:
	// immediate submit structures
	vk::Fence m_imm_fence;
	vk::CommandBuffer m_imm_command_buffer;
	vk::CommandPool   m_imm_command_pool;

};

}

#endif
