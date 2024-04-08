#ifndef GRAPHICS_DEBUG_MESSENGER_UTILS_HEADER
#define GRAPHICS_DEBUG_MESSENGER_UTILS_HEADER

#include <vulkan/vulkan.hpp>
#include <SDL_vulkan.h>

#ifdef APP_USE_VULKAN_DEBUG_REPORT

vk::DebugUtilsMessengerEXT debug_messenger;

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


static std::vector<const char*> validation_layers = {
	"VK_LAYER_KHRONOS_validation"
};

VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback( VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* pCallback, void* pUser_data)
{
    SDL_LogDebug(SDL_LOG_CATEGORY_RENDER, "Validation layer: %s\n", pCallback->pMessage);
    return VK_FALSE;
}

bool check_validation_layer_support()
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

void create_debug_utils_messenger_EXT(const vk::Instance& instance) {
    

    pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(instance.getProcAddr("vkCreateDebugUtilsMessengerEXT"));
    if (pfnVkCreateDebugUtilsMessengerEXT == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "GetInstanceProcAddr: Unable to find pfnVkCreateDebugUtilsMessengerEXT function.\n");

        std::exit(1);
    }
    pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(instance.getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
    if (pfnVkDestroyDebugUtilsMessengerEXT == nullptr) 
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "GetInstanceProcAddr: Unable to find pfnVkDestroyDebugUtilsMessengerEXT function.\n");

        std::exit(1);
    }

    vk::DebugUtilsMessengerCreateInfoEXT create_info({});
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


#endif // !GRAPHICS_DEBUG_MESSENGER_UTILS_HEADER
