#ifndef GRAPHICS_RENDERER_UTIL_QUEUE_HEADER
#define GRAPHICS_RENDERER_UTIL_QUEUE_HEADER

#include <vulkan/vulkan.hpp>

#include <deque>
#include <functional>

namespace Ui::vkutil
{
struct QueueBundle
{
	vk::Queue queue;
	uint32_t family{ 0 };

	operator vk::Queue() { return queue; }
};
}

#endif // GRAPHICS_RENDERER_UTIL_QUEUE_HEADER