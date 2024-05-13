#include "SDLWindow.hpp"


#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

namespace Ui
{


SDLWindow::SDLWindow(const std::string& title, int w, int h)
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(
		SDL_WINDOW_VULKAN |
		SDL_WINDOW_RESIZABLE
		);

	m_window = SDL_CreateWindow(
		title.c_str(),
		w,
		h,
		window_flags
	);
}

SDLWindow::~SDLWindow()
{
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

WindowExtent SDLWindow::GetDimensions() const
{
	int w, h;
	SDL_GetWindowSize(m_window, &w, &h);

	return { w , h };
}

void SDLWindow::GetDimensions(int* w, int* h) const
{
	SDL_GetWindowSize(m_window, w, h);
}


VkSurfaceKHR SDLWindow::CreateVulkanSurface(VkInstance instance)
{
	VkSurfaceKHR surface;
	SDL_Vulkan_CreateSurface(m_window, instance, NULL, &surface);
	return surface;
}


uint32_t SDLWindow::GetWindowID()
{
	return SDL_GetWindowID(m_window);
}
void SDLWindow::InitImguiForVulkan()
{
	ImGui_ImplSDL3_InitForVulkan(m_window);
}
}