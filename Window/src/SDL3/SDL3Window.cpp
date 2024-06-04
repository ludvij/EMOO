#include "SDL3/SDL3Window.hpp"


#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <bit>
#include <cstdint>

namespace Window
{


SDL3Window::SDL3Window(const std::string& title, int w, int h)
{

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

SDL3Window::~SDL3Window()
{
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

WindowExtent SDL3Window::GetDimensions() const
{
	int w, h;
	SDL_GetWindowSize(m_window, &w, &h);

	return { w , h };
}

void SDL3Window::GetDimensions(int* w, int* h) const
{
	SDL_GetWindowSize(m_window, w, h);
}


VkSurfaceKHR SDL3Window::CreateVulkanSurface(VkInstance instance)
{
	VkSurfaceKHR surface;
	SDL_Vulkan_CreateSurface(m_window, instance, NULL, &surface);
	return surface;
}


uint32_t SDL3Window::GetWindowID()
{
	return SDL_GetWindowID(m_window);
}
void SDL3Window::InitImguiForVulkan()
{
	ImGui_ImplSDL3_InitForVulkan(m_window);
}
void SDL3Window::ShutdownImGuiWindow()
{
	ImGui_ImplSDL3_Shutdown();
}
void SDL3Window::ProcessEventForImGui(void* event)
{
	ImGui_ImplSDL3_ProcessEvent(static_cast<SDL_Event*>( event ));
}
void SDL3Window::BeginImGuiFrame()
{
	ImGui_ImplSDL3_NewFrame();
}
}