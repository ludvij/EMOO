#ifndef GRAPHICS_RENDERER_HEADER
#define GRAPHICS_RENDERER_HEADER

#include <vulkan/vulkan.h>
#include <SDL.h>
#include <SDL_vulkan.h>


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

	void cleanupVulkan();
	void cleanupSdl();
private:
	SDL_Window* m_window = nullptr;

	VkInstance m_vk_instance = VK_NULL_HANDLE;
	VkAllocationCallbacks* m_vk_allocators = nullptr;
};
}
#endif
