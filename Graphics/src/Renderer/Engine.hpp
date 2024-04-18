#ifndef GRAPHICS_ENGINE_HEADER
#define GRAPHICS_ENGINE_HEADER

#include "Vulkan/Core.hpp"

#include "Vulkan/vkutil/Swapchain.hpp"
#include "Vulkan/vkutil/Queue.hpp"
#include "Vulkan/vkutil/Images.hpp"
#include "Vulkan/vkutil/Pipelines.hpp"


#include "Vulkan/Descriptors.hpp"
#include "Vulkan/Loader.hpp"


struct SDL_Window;

namespace Ui
{
using namespace Detail;

namespace Detail {

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

template<size_t N>
struct ComputePushConstants
{
	std::array<glm::vec4, N> data;
};

struct ComputeEffect
{
	std::string_view name;

	vk::Pipeline       pipeline;
	vk::PipelineLayout layout;

	ComputePushConstants<4> data{};
};
}


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

	GPUMeshBuffers UploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices);

private:
	void init();
	void cleanup();

	void draw();

	void draw_background(vk::CommandBuffer cmd);

	void draw_geometry(vk::CommandBuffer cmd);

	void resize_swapchain();


private:

	bool m_initialised{ false };
	uint32_t m_frame_number{ 0 };
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

	void init_background_pipeline();
	void init_mesh_pipeline();

	void init_default_data();


	void create_swapchain(uint32_t width, uint32_t height);
	void destroy_swapchain();

	FrameData& get_current_frame() { return m_frames[m_frame_number % FRAME_OVERLAP]; }

	AllocatedBuffer create_buffer(size_t alloc_size, vk::BufferUsageFlags usage, VmaMemoryUsage memory_usage);
	void destroy_buffer(const AllocatedBuffer& buffer);

	AllocatedImage create_image(vk::Extent3D size, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped = false);
	AllocatedImage create_image(void* data, vk::Extent3D size, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped = false);
	void destroy_image(const AllocatedImage& img);


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
	DeletionQueue m_deletion_queue;

	// memory allocator
	VmaAllocator m_allocator{ nullptr };

	// image to send to swapchain
	AllocatedImage m_draw_image;
	AllocatedImage m_depth_image;
	vk::Extent2D           m_draw_extent;
	float m_render_scale = 1.0f;

	// descriptors for compute shaders
	DescriptorAllocatorGrowable m_descriptor_allocator;
	vk::DescriptorSet           m_draw_image_descriptors;
	vk::DescriptorSetLayout     m_draw_image_descriptor_layout;
	vk::DescriptorSetLayout     m_scene_data_descriptor_layout;

	GPUSceneData m_scene_data;

	vk::PipelineLayout m_gradient_pipeline_layout;


	vk::PipelineLayout m_mesh_pipeline_layout;
	vk::Pipeline       m_mesh_pipeline;

	std::vector<std::shared_ptr<MeshAsset>> m_test_meshes;

private:

	void init_imgui();

	void immediate_submit(std::function<void(vk::CommandBuffer cmd)>&& function);

	void draw_imgui(vk::CommandBuffer cmd, vk::ImageView view);


private:
	// immediate submit structures
	vk::Fence m_imm_fence;
	vk::CommandBuffer m_imm_command_buffer;
	vk::CommandPool   m_imm_command_pool;

	std::vector<ComputeEffect> m_background_effects;
	int m_current_background_effect{ 0 };

};

}

#endif
