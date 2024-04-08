#ifndef GRAPHICS_RENDERER_HEADER
#define GRAPHICS_RENDERER_HEADER

#include <vulkan/vulkan.hpp>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vector>


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
	void initWindow();
	void initVulkan();
		void createVulkanInstance();
		void createSurface();
		void selectVulkanPhysicalDevice();
		void selectLogicalDevice();

	void cleanupVulkan();
	void cleanupSdl();

	bool is_device_suitable(const vk::PhysicalDevice& device) const;


private:
	SDL_Window* m_window = nullptr;

	vk::Instance       m_instance;
	vk::PhysicalDevice m_physical_device;
	vk::Device         m_device;
	vk::Queue          m_graphics_queue;
	vk::Queue          m_presents_queue;
	vk::SurfaceKHR     m_surface;

	std::vector<const char*> m_device_extensions = {
		vk::KHRSwapchainExtensionName
	};
};
}
#endif
