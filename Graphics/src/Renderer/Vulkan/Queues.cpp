#include "Queues.hpp"

#include <vulkan/vulkan.hpp>

namespace Ui::u
{
QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice device, const vk::SurfaceKHR surface)
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
}