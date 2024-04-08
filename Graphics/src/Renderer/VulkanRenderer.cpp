// This code is written with help from https://vulkan-tutorial.com
// but instead of using GLFW i'm using SDL2, to get some help with the sdl vulkan functions
// I used the Dear ImGui SDL2 Vulkan example source code


#include "VulkanRenderer.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>
#include <optional>

#include <vulkan/vulkan.hpp>


struct QueueFamilyIndices 
{
    std::optional<uint32_t> graphics_family;

    bool IsComplete() const
    {
        return graphics_family.has_value();
    }
};

static QueueFamilyIndices find_queue_families(vk::PhysicalDevice device)
{
    QueueFamilyIndices indices;
    uint32_t queue_family_count = 0;
    device.getQueueFamilyProperties( &queue_family_count, nullptr);
    std::vector<vk::QueueFamilyProperties> queue_families(queue_family_count);
    device.getQueueFamilyProperties(&queue_family_count, queue_families.data());

    for (uint32_t i = 0; i < queue_families.size(); i++) 
    {
        if (queue_families[i].queueFlags & vk::QueueFlagBits::eGraphics)
        {
            indices.graphics_family = i;
            if (indices.IsComplete())
            {
                break;
            }
        }
    }

    return indices;
}

static bool is_extension_available(const std::vector<vk::ExtensionProperties>& properties, const char* extension)
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


static bool is_device_suitable(vk::PhysicalDevice device)
{
    QueueFamilyIndices indices = find_queue_families(device);

    return indices.IsComplete();
}

#ifdef NES_EMU_DEBUG
    #define APP_USE_VULKAN_DEBUG_REPORT
#endif


// messy code that I'm not going to change
#ifdef APP_USE_VULKAN_DEBUG_REPORT


PFN_vkCreateDebugUtilsMessengerEXT  pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger )
{
    return pfnVkCreateDebugUtilsMessengerEXT( instance, pCreateInfo, pAllocator, pMessenger );
}

VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, VkAllocationCallbacks const * pAllocator )
{
    return pfnVkDestroyDebugUtilsMessengerEXT( instance, messenger, pAllocator );
}

static vk::DebugUtilsMessengerEXT debug_messenger;

static std::vector<const char*> validation_layers = {
	"VK_LAYER_KHRONOS_validation"
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback( VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* pCallback, void* pUser_data)
{
    std::cerr << "validation layer: " << pCallback->pMessage << std::endl;
    return VK_FALSE;
}

static bool check_validation_layer_support()
{
    std::vector<vk::LayerProperties> available_layers = vk::enumerateInstanceLayerProperties();

    uint32_t found = 0;
    for (const auto& layerName : validation_layers) 
    {
        for (const auto& layerProperties : available_layers) 
        {
            if (std::strcmp(layerName, layerProperties.layerName) == 0) 
            {
                found++;
            }
        }
    }
    return found == validation_layers.size();
}

static void create_debug_utils_messenger_EXT(vk::Instance instance) {
    

    pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
    if (pfnVkCreateDebugUtilsMessengerEXT == nullptr)
    {
        std::cerr << "GetInstanceProcAddr: Unable to find pfnVkCreateDebugUtilsMessengerEXT function.\n" ;
        std::exit(1);
    }
    pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
    if (pfnVkDestroyDebugUtilsMessengerEXT == nullptr) 
    {
        std::cerr << "GetInstanceProcAddr: Unable to find pfnVkDestroyDebugUtilsMessengerEXT function.\n";
        std::exit(1);
    }

    vk::DebugUtilsMessengerCreateInfoEXT create_info({});
    create_info.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;
    create_info.messageSeverity = 
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | 
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
    create_info.messageType = 
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
    create_info.pfnUserCallback = debug_callback;
    debug_messenger = instance.createDebugUtilsMessengerEXT(create_info);
}




#endif


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
        std::cerr << "Error: SDL_CreateWindow(): " << SDL_GetError() << '\n';
        std::exit(1);
    }
}

void Renderer::initVulkan()
{
#ifdef APP_USE_VULKAN_DEBUG_REPORT
    if (!check_validation_layer_support()) 
    {
        std::cerr << "Validation layer support requested, but not available\n";
        std::exit(1);
    }
#endif // APP_USE_VULKAN_DEBUG_REPORT

    createVulkanInstance();
    
#ifdef APP_USE_VULKAN_DEBUG_REPORT
    create_debug_utils_messenger_EXT(m_instance);
#endif // APP_USE_VULKAN_DEBUG_REPORT

    selectVulkanPhysicalDevice();

}

void Renderer::createVulkanInstance()
{
    // creating vulkan instance
    vk::ApplicationInfo app_info("NES EMULATOR TFG Luis Vijande", 1, "NES Emulator Rendering Engine", 1, VK_API_VERSION_1_1);

    std::vector<const char*> extensions;
    uint32_t extensions_count = 0;
    SDL_Vulkan_GetInstanceExtensions(m_window, &extensions_count, nullptr);
    extensions.resize(extensions_count);
    SDL_Vulkan_GetInstanceExtensions(m_window, &extensions_count, extensions.data());

#ifdef APP_USE_VULKAN_DEBUG_REPORT
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

#ifdef APP_USE_VULKAN_DEBUG_REPORT
    vk::InstanceCreateInfo create_info(vk::InstanceCreateFlags(), &app_info, validation_layers, extensions);
#else
    vk::InstanceCreateInfo create_info(vk::InstanceCreateFlags(), &app_info, nullptr, extensions);
#endif

    m_instance = vk::createInstance(create_info);
}

void Renderer::selectVulkanPhysicalDevice()
{
    std::vector<vk::PhysicalDevice> devices = m_instance.enumeratePhysicalDevices();
    if (devices.empty())
    {
        std::cerr << "failed to find GPUs with Vulkan support!\n";
        std::exit(1);
    }

    for (vk::PhysicalDevice& device : devices)
    {
        auto properties = device.getProperties();
        if (is_device_suitable(device) && properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
        {
            m_physical_device = device;
            return;
        }
    }

    // Use first GPU (Integrated) is a Discrete one is not available.
    m_physical_device = devices[0];
}

void Renderer::selectLogicalDevice()
{
    QueueFamilyIndices indices = find_queue_families(m_physical_device);
    float queue_priority = 1.0f;

    vk::DeviceQueueCreateInfo queue_create_info(vk::DeviceQueueCreateFlags(),
                                                static_cast<uint32_t>(indices.graphics_family.value()),
                                                1,
                                                &queue_priority);

    vk::PhysicalDeviceFeatures device_features{};

    // Enumerate physical device extension
    std::vector<const char*> extensions;
    extensions.push_back("VK_KHR_swapchain");


#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
    std::vector<vk::ExtensionProperties> properties = m_physical_device.enumerateDeviceExtensionProperties();
    if (is_extension_available(properties, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
        device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

    vk::DeviceCreateInfo create_info(vk::DeviceCreateFlags(), queue_create_info, extensions);
    create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();
    create_info.pEnabledFeatures = &device_features;

    m_device = m_physical_device.createDevice(create_info);

    m_graphics_queue = m_device.getQueue(indices.graphics_family.value(), 0);
}


// cleans the vulkan things
void Renderer::cleanupVulkan()
{
#ifdef APP_USE_VULKAN_DEBUG_REPORT
    m_instance.destroyDebugUtilsMessengerEXT(debug_messenger);
#endif // APP_USE_VULKAN_DEBUG_REPORT
    m_device.destroy();
    m_instance.destroy();
}

void Renderer::cleanupSdl()
{
    SDL_DestroyWindow(m_window);

    SDL_Quit();
}

}