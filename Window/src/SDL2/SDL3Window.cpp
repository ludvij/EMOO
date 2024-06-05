#include "SDL2/SDL2Window.hpp"


#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_vulkan.h>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <bit>
#include <cstdint>

namespace Window
{


SDL2Window::SDL2Window(const std::string& title, int w, int h)
{

	SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(
		SDL_WINDOW_VULKAN |
		SDL_WINDOW_RESIZABLE
		);

	m_window = SDL_CreateWindow(
		title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		w,
		h,
		window_flags
	);
}

SDL2Window::~SDL2Window()
{
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

WindowExtent SDL2Window::GetDimensions() const
{
	int w, h;
	SDL_GetWindowSize(m_window, &w, &h);

	return { w , h };
}

void SDL2Window::GetDimensions(int* w, int* h) const
{
	SDL_GetWindowSize(m_window, w, h);
}


VkSurfaceKHR SDL2Window::CreateVulkanSurface(VkInstance instance)
{
	VkSurfaceKHR surface;
	SDL_Vulkan_CreateSurface(m_window, instance, &surface);
	return surface;
}


uint32_t SDL2Window::GetWindowID()
{
	return SDL_GetWindowID(m_window);
}
void SDL2Window::InitImguiForVulkan()
{
	ImGui_ImplSDL2_InitForVulkan(m_window);
}
void SDL2Window::ShutdownImGuiWindow()
{
	ImGui_ImplSDL2_Shutdown();
}
void SDL2Window::ProcessEventForImGui(void* event)
{
	ImGui_ImplSDL2_ProcessEvent(static_cast<SDL_Event*>( event ));
}
void SDL2Window::BeginImGuiFrame()
{
	ImGui_ImplSDL2_NewFrame();
}
}