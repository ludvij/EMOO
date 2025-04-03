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

static Event get_equivalent_from_sdl(uint32_t event)
{
	typedef Window::Event E;
	switch (event)
	{
	case SDL_EVENT_WINDOW_RESIZED:         return E::RESIZED;
	case SDL_EVENT_WINDOW_MINIMIZED:       return E::MINIMIZED;
	case SDL_EVENT_WINDOW_RESTORED:        return E::RESTORED;
	case SDL_EVENT_WINDOW_MAXIMIZED:       return E::MAXIMIZED;
	case SDL_EVENT_WINDOW_CLOSE_REQUESTED: return E::CLOSE;
	case SDL_EVENT_WINDOW_MOVED:           return E::MOVED;
	case SDL_EVENT_WINDOW_MOUSE_LEAVE:     return E::FOCUS_LOST_MOUSE;
	case SDL_EVENT_WINDOW_MOUSE_ENTER:     return E::FOCUS_GAIN_MOUSE;
	case SDL_EVENT_WINDOW_FOCUS_LOST:      return E::FOCUS_LOST_KEYBOARD;
	case SDL_EVENT_WINDOW_FOCUS_GAINED:    return E::FOCUS_GAIN_KEYBOARD;
	default:                               return E::NONE;
	}
}


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


void* SDL3Window::CreateRenderSurface(void* instance)
{
	VkSurfaceKHR surface;
	SDL_Vulkan_CreateSurface(m_window, static_cast<VkInstance>( instance ), NULL, &surface);
	return surface;
}


uint32_t SDL3Window::GetWindowID()
{
	return SDL_GetWindowID(m_window);
}
void SDL3Window::InitImguiForRenderer()
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
void SDL3Window::ProcessEvents(void* event)
{
	SDL_Event* e = static_cast<SDL_Event*>( event );
	ImGui_ImplSDL3_ProcessEvent(e);
	if (!( e->type >= SDL_EVENT_WINDOW_FIRST && e->type <= SDL_EVENT_WINDOW_LAST ))
	{
		return;
	}

	Event my_event = get_equivalent_from_sdl(e->window.type);
	if (m_event_fn.contains(my_event))
	{
		m_event_fn.at(my_event)( this, event );
	}
}
void SDL3Window::Maximize(bool maximize)
{
	if (maximize)
	{
		SDL_MaximizeWindow(m_window);
	}
	else
	{
		// do nothing yet
	}
}
}