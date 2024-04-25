#ifndef GRAPHICS_RENDERER_UTIL_SWAPCHAIN_HEADER
#define GRAPHICS_RENDERER_UTIL_SWAPCHAIN_HEADER

#include <vulkan/vulkan.hpp>

namespace Ui::vkutil
{

struct QueueBundle
{
	vk::Queue queue;
	uint32_t family{ 0 };

	operator vk::Queue()
	{
		return queue;
	}
};

struct ImageFrame
{
	vk::Image image;
	vk::ImageView view;
};


struct SwapchainBundle
{
	vk::SwapchainKHR swapchain;
	vk::Format format{ vk::Format::eUndefined };
	vk::Extent2D extent;

	std::vector<ImageFrame> frames;

	operator vk::SwapchainKHR()
	{
		return swapchain;
	}
};
}

#endif // !GRAPHICS_RENDERER_UTIL_SWAPCHAIN_HEADER