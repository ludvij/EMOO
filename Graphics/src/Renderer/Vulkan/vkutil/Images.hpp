#ifndef GRAPHICS_RENDERER_UTIL_IMAGES_HEADER
#define GRAPHICS_RENDERER_UTIL_IMAGES_HEADER

#include <vulkan/vulkan.hpp>

#include "Renderer/Vulkan/Initializers.hpp"

namespace Ui::vkutil
{

void transition_image(vk::CommandBuffer cmd, vk::Image image, vk::ImageLayout current_layout, vk::ImageLayout new_layout);

void copy_image_to_image(vk::CommandBuffer cmd, vk::Image src, vk::Image dst, vk::Extent2D src_size, vk::Extent2D dst_size);

}
#endif // !GRAPHICS_RENDERER_IMAGES_HEADER
