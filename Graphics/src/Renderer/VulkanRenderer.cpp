// This code is written with help from https://vulkan-tutorial.com
// but instead of using GLFW i'm using SDL2, to get some help with the sdl vulkan functions
// I used the Dear ImGui SDL2 Vulkan example source code


#include "VulkanRenderer.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>
#include <set>
#include <optional>

#include <vulkan/vulkan.hpp>

#ifdef NES_EMU_DEBUG
    #define APP_USE_VULKAN_DEBUG_REPORT
    // including a cpp file, I know, cringe, but it's only purpose is to get junk out of this file, 
    // so it's okay
    #include "DebugMessengerUtils.cpp"
#endif


struct QueueFamilyIndices 
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> presents_family;

    bool IsComplete() const
    {
        return graphics_family.has_value() 
            && presents_family.has_value();
    }
};

static QueueFamilyIndices find_queue_families(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)
{
    QueueFamilyIndices indices;
    const std::vector<vk::QueueFamilyProperties> queue_families = device.getQueueFamilyProperties();

    for (uint32_t i = 0; i < queue_families.size(); i++) 
    {
        if (queue_families[i].queueFlags & vk::QueueFlagBits::eGraphics)
        {
            indices.graphics_family = i;

            if(device.getSurfaceSupportKHR(i, surface))
            {
                indices.presents_family = i;
            }

            if (indices.IsComplete())
            {
                break;
            }
        }
    }

    return indices;
}

bool is_extension_available(const std::vector<vk::ExtensionProperties>& properties, const char* extension)
{
    for (const vk::ExtensionProperties& p : properties) 
    {
        if (strcmp(p.extensionName, extension) == 0)
        {
            return true;
        }
    }
    return false;
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
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error %s\n", SDL_GetError());
		std::exit(1);
	}

    SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(
        SDL_WINDOW_VULKAN | 
        SDL_WINDOW_RESIZABLE | 
        SDL_WINDOW_ALLOW_HIGHDPI
    );

	m_window = SDL_CreateWindow("Vulkan Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, window_flags);
    if (m_window == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        std::exit(1);
    }
}

void Renderer::initVulkan()
{
#ifdef APP_USE_VULKAN_DEBUG_REPORT
    if (!check_validation_layer_support()) 
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Validation layer support requested, but not available\n");
        std::exit(1);
    }
#endif // APP_USE_VULKAN_DEBUG_REPORT

    createVulkanInstance();
    
#ifdef APP_USE_VULKAN_DEBUG_REPORT
    create_debug_utils_messenger_EXT(m_instance);
#endif // APP_USE_VULKAN_DEBUG_REPORT
    createSurface();
    selectVulkanPhysicalDevice();
    selectLogicalDevice();
}

bool Renderer::is_device_suitable(const vk::PhysicalDevice& device) const
{
    const QueueFamilyIndices indices = find_queue_families(device, m_surface);

    const auto available_extensions = device.enumerateDeviceExtensionProperties();
    std::set<std::string> required_extensions(m_device_extensions.begin(), m_device_extensions.end());

    for (const auto& extension : available_extensions)
    {
        required_extensions.erase(extension.extensionName);
    }

    return indices.IsComplete() && required_extensions.empty();
}

void Renderer::createVulkanInstance()
{
    // creating vulkan instance
    const vk::ApplicationInfo app_info("NES EMULATOR TFG Luis Vijande", 1, "NES Emulator Rendering Engine", 1, VK_API_VERSION_1_1);

    std::vector<const char*> extensions;
    uint32_t extensions_count = 0;
    SDL_Vulkan_GetInstanceExtensions(m_window, &extensions_count, nullptr);
    extensions.resize(extensions_count);
    SDL_Vulkan_GetInstanceExtensions(m_window, &extensions_count, extensions.data());

#ifdef APP_USE_VULKAN_DEBUG_REPORT
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    vk::InstanceCreateInfo create_info(vk::InstanceCreateFlags(), &app_info, validation_layers, extensions);
#else
    vk::InstanceCreateInfo create_info(vk::InstanceCreateFlags(), &app_info, nullptr, extensions);
#endif

    m_instance = vk::createInstance(create_info);
}

void Renderer::selectVulkanPhysicalDevice()
{
    const std::vector<vk::PhysicalDevice> devices = m_instance.enumeratePhysicalDevices();
    if (devices.empty())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to find GPUs with Vulkan support!\n");
        std::exit(1);
    }

    for (const auto& device : devices)
    {
        const auto properties = device.getProperties();
        if (is_device_suitable(device) && properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
        {
            m_physical_device = device;
            return;
        }
    }

    // Use first GPU (Integrated) if a Discrete one is not available.
    m_physical_device = devices[0];
}

void Renderer::createSurface()
{
    VkSurfaceKHR surface;
    if (SDL_Vulkan_CreateSurface(m_window, m_instance, &surface) == SDL_FALSE)
    {
        std::exit(1);
    }

    m_surface = surface;
}

void Renderer::selectLogicalDevice()
{
    const QueueFamilyIndices indices = find_queue_families(m_physical_device, m_surface);
    std::vector<vk::DeviceQueueCreateInfo> queue_create_infos;
    const std::set<uint32_t> unique_queue_families = {
        indices.graphics_family.value(), 
        indices.presents_family.value()
    };

    const float queue_priority = 1.0f;
    for(uint32_t queue_family : unique_queue_families)
    {
        const vk::DeviceQueueCreateInfo create_info(vk::DeviceQueueCreateFlags(), queue_family, 1, &queue_priority);

        queue_create_infos.push_back(create_info);
    }

    const vk::PhysicalDeviceFeatures device_features{};

    // Enumerate physical device extension

    const vk::DeviceCreateInfo create_info(vk::DeviceCreateFlags(), queue_create_infos, {}, m_device_extensions, &device_features);


    m_device = m_physical_device.createDevice(create_info);

    m_graphics_queue = m_device.getQueue(indices.graphics_family.value(), 0);
    m_presents_queue = m_device.getQueue(indices.presents_family.value(), 0);
}


// cleans the vulkan things
void Renderer::cleanupVulkan()
{
#ifdef APP_USE_VULKAN_DEBUG_REPORT
    m_instance.destroyDebugUtilsMessengerEXT(debug_messenger);
#endif // APP_USE_VULKAN_DEBUG_REPORT
    m_instance.destroySurfaceKHR(m_surface);
    m_device.destroy();
    m_instance.destroy();
}

void Renderer::cleanupSdl()
{
    SDL_DestroyWindow(m_window);

    SDL_Quit();
}

}