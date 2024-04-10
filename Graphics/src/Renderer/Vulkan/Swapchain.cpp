#include "Swapchain.hpp"


namespace Ui::u
{
SwapChainSupportDetails querySwapchainSupport(const vk::PhysicalDevice device, const vk::SurfaceKHR surface)
{
    SwapChainSupportDetails details;

    details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
    details.formats = device.getSurfaceFormatsKHR(surface);
    details.present_modes = device.getSurfacePresentModesKHR(surface);

    return details;
}

vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& available_formats) 
{
    for(const auto& available_format : available_formats)
    {
        if (available_format.format == vk::Format::eB8G8R8A8Srgb && available_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return available_format;
        }
    }

    return available_formats[0];
}

vk::PresentModeKHR chooseSwapSurfacePresentMode(const std::vector<vk::PresentModeKHR>& available_present_modes)
{
    for (const auto& available_present_mode : available_present_modes)
    {
        if (available_present_mode == vk::PresentModeKHR::eMailbox)
        {
            return available_present_mode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}


vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, SDL_Window* window)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    int w, h;
    SDL_Vulkan_GetDrawableSize(window, &w, &h);

    vk::Extent2D actual_extent(static_cast<uint32_t>(w), static_cast<uint32_t>(h));
        
    actual_extent.width = std::clamp(
        actual_extent.width, 
        capabilities.minImageExtent.width, 
        capabilities.maxImageExtent.width
    );
    actual_extent.height = std::clamp(
        actual_extent.height,
        capabilities.minImageExtent.height,
        capabilities.maxImageExtent.height
    );

    return actual_extent;
}
}