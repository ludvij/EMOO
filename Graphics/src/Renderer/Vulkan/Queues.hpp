#ifndef GRAPHICS_QUEUES_HEADER
#define GRAPHICS_QUEUES_HEADER

#include <optional>

#include <vulkan/vulkan.hpp>

namespace Ui::u
{
struct QueueFamilyIndices 
{
	std::optional<uint32_t> graphics_family = std::nullopt;
	std::optional<uint32_t> presents_family = std::nullopt;

	bool IsComplete() const
	{
		return graphics_family.has_value() 
			&& presents_family.has_value();
	}
};


QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice device, const vk::SurfaceKHR surface);

}

#endif // !GRAPHICS_QUEUES_HEADER
