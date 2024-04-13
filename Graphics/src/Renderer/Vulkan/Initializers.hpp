#ifndef GRAPHICS_RENDERER_INITIALIZERS_HEADER
#define GRAPHICS_RENDERER_INITIALIZERS_HEADER

#include <vulkan/vulkan.hpp>

namespace Ui::vkinit
{

vk::CommandPoolCreateInfo command_pool_create_info(const uint32_t queue_family_index, const vk::CommandPoolCreateFlags flags = {});

vk::CommandBufferAllocateInfo command_buffer_allocate_info(const vk::CommandPool pool, const uint32_t count = 1);

vk::FenceCreateInfo fence_create_info(const vk::FenceCreateFlagBits flags = {});

vk::SemaphoreCreateInfo semaphore_create_info(const vk::SemaphoreCreateFlags flags = {});

vk::CommandBufferBeginInfo command_buffer_begin_info(const vk::CommandBufferUsageFlags flags = {});

vk::ImageSubresourceRange image_subresource_range(vk::ImageAspectFlags flags = {});

vk::SemaphoreSubmitInfo semaphire_submit_info(const vk::PipelineStageFlags2 stage_mask, const vk::Semaphore semaphore);

vk::CommandBufferSubmitInfo command_buffer_submit_info(const vk::CommandBuffer cmd);

vk::SubmitInfo2 submit_info(vk::CommandBufferSubmitInfo* cmd, vk::SemaphoreSubmitInfo* signal_semaphore, vk::SemaphoreSubmitInfo* wait_semaphore);

vk::ImageCreateInfo image_create_info(const vk::Format format, vk::ImageUsageFlags usage_flags, vk::Extent3D extent);

vk::ImageViewCreateInfo image_view_create_info(vk::Format format, vk::Image image, vk::ImageAspectFlags aspect_flags);

vk::RenderingAttachmentInfo attachment_info(vk::ImageView view, vk::ClearValue* clear, vk::ImageLayout layout = vk::ImageLayout::eColorAttachmentOptimal);

vk::RenderingInfo rendering_info(vk::Extent2D render_extent, vk::RenderingAttachmentInfo* color_attachment, vk::RenderingAttachmentInfo* depth_attachment);

}
#endif // !GRAPHICS_RENDERER_INITIALIZERS_HEADER
