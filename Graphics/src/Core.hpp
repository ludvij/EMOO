#ifndef GRAPHCIS_CORE_HEADER
#define GRAPHICS_CORE_HEADER

#include <stdlib.h>         // abort
#include <iostream>

include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_vulkan.h>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>

static void check_vk_result(VkResult err)
{
	if (err == 0) 
	{
		return;
	}
	std::cerr << "[vulkan] Error: VkResult = %d\n";
	if (err < 0) 
	{
		std::abort();
	}
}

#endif