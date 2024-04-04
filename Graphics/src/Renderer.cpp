#include "Renderer.hpp"
#include <cstdlib>
#include <iostream>
#include <vector>

#include <vulkan/vulkan.h>

static void check_vk_result(VkResult err)
{
    if (err == 0)
        return;
    std::cerr << "[vulkan] Error: VkResult = " << err << '\n';

    if (err < 0)
        std::abort();
}

namespace Ui
{
Renderer::Renderer()
{
    Init();
}

Renderer::~Renderer()
{
    Cleanup();
}

void Renderer::Init()
{
    initWindow();
    initVulkan();
}

void Renderer::Cleanup()
{
    cleanupVulkan();
    cleanupSdl();
}

void Renderer::initWindow()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) 
	{
		std::cerr << "Error " << SDL_GetError() << '\n';
		std::abort();
	}

    SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(
        SDL_WINDOW_VULKAN | 
        SDL_WINDOW_RESIZABLE | 
        SDL_WINDOW_ALLOW_HIGHDPI
    );

	m_window = SDL_CreateWindow("Vulkan Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, window_flags);
    if (m_window == nullptr)
    {
        std::cerr << "Error: SDL_CreateWindow(): " << SDL_GetError() << '\n';
        std::abort();
    }
}

void Renderer::initVulkan()
{
    // creating vulkan instance
    VkApplicationInfo appInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Application name",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "NES emulator Rendering engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_3
    };

    std::vector<const char*> extensions;
    uint32_t extensions_count = 0;
    SDL_Vulkan_GetInstanceExtensions(m_window, &extensions_count, nullptr);
    extensions.resize(extensions_count);
    SDL_Vulkan_GetInstanceExtensions(m_window, &extensions_count, extensions.data());

    VkInstanceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = 0,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    VkResult err = vkCreateInstance(&createInfo, m_vk_allocators, &m_vk_instance);
    check_vk_result(err);
}

void Renderer::cleanupVulkan()
{
    vkDestroyInstance(m_vk_instance, m_vk_allocators);
}

void Renderer::cleanupSdl()
{
    SDL_DestroyWindow(m_window);

    SDL_Quit();
}

}