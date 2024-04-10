#ifndef GRAPHICS_SWAPCHAIN_HEADER
#define GRAPHICS_SWAPCHAIN_HEADER

#include <vector>
#include <vulkan/vulkan.hpp>
#include <SDL.h>
#include <SDL_vulkan.h>

namespace Ui::u
{

struct SwapChainSupportDetails 
{
	vk::SurfaceCapabilitiesKHR capabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR> present_modes;
};


struct SwapchainImageFrame
{
	vk::Image image;
	vk::ImageView view;
};


struct SwapchainBundle
{
	operator vk::SwapchainKHR() { return swapchain; };

	vk::SwapchainKHR                 swapchain;
	vk::Extent2D                     extent;
	vk::Format                       format;
	std::vector<SwapchainImageFrame> frames;
};



SwapChainSupportDetails querySwapchainSupport(const vk::PhysicalDevice device, const vk::SurfaceKHR surface);

vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& available_formats);

vk::PresentModeKHR chooseSwapSurfacePresentMode(const std::vector<vk::PresentModeKHR>& available_present_modes);

vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, SDL_Window* window);


}

#endif // !GRAPHICS_SWAPCHAIN_HEADER