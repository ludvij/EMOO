// This code is written with help from https://vulkan-tutorial.com
// but instead of using GLFW i'm using SDL2, to get some help with the sdl vulkan functions
// I used the Dear ImGui SDL2 Vulkan example source code


#include "VulkanRenderer.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>
#include <set>
#include <limits>
#include <algorithm>
#include "Window/Window.hpp"

#define LUD_SLURPER_IMPLEMENTATION
#include "Util/Slurper.hpp"

#include <vulkan/vulkan.hpp>

#ifdef NES_EMU_DEBUG
    #define APP_USE_VULKAN_DEBUG_REPORT
    // including a cpp file, I know, cringe, but it's only purpose is to get junk out of this file, 
    // so it's okay
    #include "Vulkan/DebugMessengerUtils.cpp"
#endif

#include "Vulkan/Swapchain.hpp"
#include "Vulkan/Queues.hpp"
#include "Vulkan/Shader.hpp"



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
    initVulkan();
}

void Renderer::Cleanup()
{
    cleanupVulkan();
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
    createSwapChain();
    createImageViews();
    createGraphicsPipeline();
}

void Renderer::createVulkanInstance()
{
    // creating vulkan instance
    const vk::ApplicationInfo app_info("NES EMULATOR TFG Luis Vijande", 1, "NES Emulator Rendering Engine", 1, VK_API_VERSION_1_1);

    std::vector<const char*> extensions;

    uint32_t extensions_count = 0;
    SDL_Vulkan_GetInstanceExtensions(Window::Get(), &extensions_count, nullptr);
    extensions.resize(extensions_count);
    SDL_Vulkan_GetInstanceExtensions(Window::Get(), &extensions_count, extensions.data());

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
        if (isDeviceSuitable(device) && properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
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
    if (SDL_Vulkan_CreateSurface(Window::Get(), m_instance, &surface) == SDL_FALSE)
    {
        std::exit(1);
    }

    m_surface = surface;
}

void Renderer::selectLogicalDevice()
{
    const u::QueueFamilyIndices indices = u::findQueueFamilies(m_physical_device, m_surface);
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


void Renderer::createSwapChain()
{
    u::SwapChainSupportDetails support = u::querySwapchainSupport(m_physical_device, m_surface);

    vk::SurfaceFormatKHR surface_format = u::chooseSwapSurfaceFormat(support.formats);
    vk::PresentModeKHR present_mode = u::chooseSwapSurfacePresentMode(support.present_modes);
    vk::Extent2D extent = u::chooseSwapExtent(support.capabilities, Window::Get());

    uint32_t image_count = support.capabilities.minImageCount + 1;
    if (support.capabilities.maxImageCount > 0 && image_count > support.capabilities.maxImageCount)
    {
        image_count = support.capabilities.maxImageCount;
    }

    vk::SwapchainCreateInfoKHR create_info(
        vk::SwapchainCreateFlagsKHR(), 
        m_surface, 
        image_count, 
        surface_format.format,
        surface_format.colorSpace,
        extent,
        1,
        vk::ImageUsageFlagBits::eColorAttachment
    );

    const u::QueueFamilyIndices indices = u::findQueueFamilies(m_physical_device, m_surface);
    uint32_t queue_family_indices[] = {indices.graphics_family.value(), indices.presents_family.value()};
    if (indices.graphics_family != indices.presents_family)
    {
        create_info.imageSharingMode = vk::SharingMode::eConcurrent;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }
    
    create_info.preTransform = support.capabilities.currentTransform;
    create_info.presentMode = present_mode;
    create_info.clipped = vk::True;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    m_swapchain = {
        .swapchain = m_device.createSwapchainKHR(create_info),
        .extent = extent,
        .format = surface_format.format,
    };
    auto images = m_device.getSwapchainImagesKHR(m_swapchain);
    m_swapchain.frames.resize(images.size());
    for(size_t i = 0; i < images.size(); i++)
    {
        m_swapchain.frames[i].image = images[i];
    }
    
}

void Renderer::createImageViews()
{

    for(size_t i = 0; i < m_swapchain.frames.size(); i++)
    {
        vk::ImageViewCreateInfo create_info(
            vk::ImageViewCreateFlags(),
            m_swapchain.frames[i].image,
            vk::ImageViewType::e2D,
            m_swapchain.format,
            {}, // all to eIdentity
            { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
        );

        m_swapchain.frames[i].view = m_device.createImageView(create_info);
    }
}

void Renderer::createGraphicsPipeline()
{
    auto vert_shader_code = Lud::Slurper::SlurpChars("Shader/SPIRV/vert.spv", std::ios::binary);
    auto frag_shader_code = Lud::Slurper::SlurpChars("Shader/SPIRV/frag.spv", std::ios::binary);

    vk::ShaderModule vert_shader_module = createShaderModule(m_device, vert_shader_code);
    vk::ShaderModule frag_shader_module = createShaderModule(m_device, frag_shader_code);

    vk::PipelineShaderStageCreateInfo vert_shader_stage_info(
        vk::PipelineShaderStageCreateFlags(),
        vk::ShaderStageFlagBits::eVertex,
        vert_shader_module,
        "main"
    );

    vk::PipelineShaderStageCreateInfo frag_shader_stage_info(
        vk::PipelineShaderStageCreateFlags(),
        vk::ShaderStageFlagBits::eFragment,
        frag_shader_module,
        "main"
    );

    vk::PipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info, frag_shader_stage_info};

    /*std::vector<vk::DynamicState> dynamic_states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

    vk::PipelineDynamicStateCreateInfo dynamic_state_info(vk::PipelineDynamicStateCreateFlags(), dynamic_states);*/

    vk::PipelineVertexInputStateCreateInfo vertex_input_info(vk::PipelineVertexInputStateCreateFlags());

    m_device.destroyShaderModule(vert_shader_module);
    m_device.destroyShaderModule(frag_shader_module);

    vk::PipelineInputAssemblyStateCreateInfo input_assembly_info(
        vk::PipelineInputAssemblyStateCreateFlags(),
        vk::PrimitiveTopology::eTriangleList,
        vk::False
    );

    vk::Viewport viewport(
        0.0f, 
        0.0f, 
        static_cast<float>(m_swapchain.extent.width),
        static_cast<float>(m_swapchain.extent.height),
        0.0f,
        1.0f
    );

    vk::Rect2D scissor({0, 0}, m_swapchain.extent);

    vk::PipelineRasterizationStateCreateInfo rasterizer_info(
        vk::PipelineRasterizationStateCreateFlags(),
        vk::False,
        vk::False,
        vk::PolygonMode::eFill,
        vk::CullModeFlagBits::eBack,
        vk::FrontFace::eClockwise,
        vk::False,
        0.0f,
        0.0f,
        0.0f,
        1.0f
    );

    vk::PipelineMultisampleStateCreateInfo multisamplig_info(
        vk::PipelineMultisampleStateCreateFlagBits(),
        vk::SampleCountFlagBits::e1,
        vk::False,
        1.0f,
        nullptr,
        vk::False,
        vk::False
    );



}


// cleans the vulkan things
void Renderer::cleanupVulkan()
{
#ifdef APP_USE_VULKAN_DEBUG_REPORT
    m_instance.destroyDebugUtilsMessengerEXT(debug_messenger);
#endif // APP_USE_VULKAN_DEBUG_REPORT
    for(const auto& image_frames : m_swapchain.frames)
    {
        m_device.destroyImageView(image_frames.view);
    }
    m_device.destroySwapchainKHR(m_swapchain);
    m_instance.destroySurfaceKHR(m_surface);
    m_device.destroy();
    m_instance.destroy();
    
}


bool Renderer::isDeviceSuitable(const vk::PhysicalDevice device) const
{
    const u::QueueFamilyIndices indices = u::findQueueFamilies(device, m_surface);

    const auto available_extensions = device.enumerateDeviceExtensionProperties();
    std::set<std::string> required_extensions(m_device_extensions.begin(), m_device_extensions.end());

    for (const auto& extension : available_extensions)
    {
        required_extensions.erase(extension.extensionName);
    }

    bool swapchain_adequate = false;
    if (required_extensions.empty())
    {
        const auto swapchain_support = u::querySwapchainSupport(device, m_surface);
        swapchain_adequate = !swapchain_support.formats.empty() && !swapchain_support.present_modes.empty();
    }
    return indices.IsComplete() && required_extensions.empty() && swapchain_adequate;
}




}