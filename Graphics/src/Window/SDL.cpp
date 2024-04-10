#include "SDL.hpp"
#include <cstdlib>

#include <vulkan/vulkan.hpp>

namespace Ui
{

SDL& SDL::Get()
{
	static SDL s_instance;
	return s_instance;
}

std::vector<const char*> SDL::GetInstanceExtensions()
{
    
}
	

SDL::~SDL()
{
    SDL_DestroyWindow(m_window);
}
void SDL::Init(const char* name, uint32_t w, uint32_t h)
{
    SDL_Init(SDL_INIT_VIDEO);
	SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(
        SDL_WINDOW_VULKAN | 
        SDL_WINDOW_RESIZABLE | 
        SDL_WINDOW_ALLOW_HIGHDPI
    );

	m_window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, window_flags);
    if (m_window == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        std::exit(1);
    }

}

}
