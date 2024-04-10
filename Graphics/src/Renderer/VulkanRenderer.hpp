#ifndef GRAPHICS_RENDERER_HEADER
#define GRAPHICS_RENDERER_HEADER

#include <vulkan/vulkan.hpp>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vector>
#include <optional>

#include "Vulkan/Swapchain.hpp"
#include "Vulkan/Queues.hpp"

namespace Ui
{
class Renderer
{
public:

	Renderer();
	~Renderer();

	void Init();
	void Cleanup();


private:
	void initVulkan();
	// vulkan init things
	void createVulkanInstance();
	void createSurface();
	void selectVulkanPhysicalDevice();
	void selectLogicalDevice();
	void createSwapChain();
	void createImageViews();
	void createGraphicsPipeline();

	void cleanupVulkan();



	bool isDeviceSuitable(const vk::PhysicalDevice device) const;

private:

	vk::Instance       m_instance;
	vk::PhysicalDevice m_physical_device;
	vk::Device         m_device;
	vk::Queue          m_graphics_queue;
	vk::Queue          m_presents_queue;
	vk::SurfaceKHR     m_surface;

	u::SwapchainBundle m_swapchain;

	std::vector<const char*> m_device_extensions{
		vk::KHRSwapchainExtensionName
	};

};
}
#endif
