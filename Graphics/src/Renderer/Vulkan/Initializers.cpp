#include "Initializers.hpp"


namespace Ui::vkinit
{


vk::CommandPoolCreateInfo command_pool_create_info(const uint32_t queue_family_index, const vk::CommandPoolCreateFlags flags)
{
	vk::CommandPoolCreateInfo info(flags, queue_family_index);
	return info;
}

vk::CommandBufferAllocateInfo command_buffer_allocate_info(const vk::CommandPool pool, const uint32_t count)
{
	vk::CommandBufferAllocateInfo info(pool, vk::CommandBufferLevel::ePrimary, count);
	return info;
}

vk::FenceCreateInfo fence_create_info(const vk::FenceCreateFlagBits flags)
{
	vk::FenceCreateInfo info(flags);
	return info;
}

vk::SemaphoreCreateInfo semaphore_create_info(const vk::SemaphoreCreateFlags flags)
{
	vk::SemaphoreCreateInfo info(flags);
	return info;
}

vk::CommandBufferBeginInfo command_buffer_begin_info(const vk::CommandBufferUsageFlags flags)
{
	vk::CommandBufferBeginInfo info(flags, nullptr);

	return info;
}

vk::ImageSubresourceRange image_subresource_range(vk::ImageAspectFlags flags)
{
	vk::ImageSubresourceRange sub_image(flags, 0, vk::RemainingMipLevels, 0, vk::RemainingArrayLayers);

	return sub_image;
}

vk::SemaphoreSubmitInfo semaphire_submit_info(const vk::PipelineStageFlags2 stage_mask, const vk::Semaphore semaphore)
{
	vk::SemaphoreSubmitInfo info(semaphore, 1, stage_mask, 0);

	return info;
}

vk::CommandBufferSubmitInfo command_buffer_submit_info(const vk::CommandBuffer cmd)
{
	vk::CommandBufferSubmitInfo info(cmd, 0);

	return info;
}

vk::SubmitInfo2 submit_info(vk::CommandBufferSubmitInfo* cmd, vk::SemaphoreSubmitInfo* signal_semaphore, vk::SemaphoreSubmitInfo* wait_semaphore)
{
	vk::SubmitInfo2 info;
	info.waitSemaphoreInfoCount = wait_semaphore == nullptr ? 0 : 1;
	info.pWaitSemaphoreInfos = wait_semaphore;

	info.commandBufferInfoCount = 1;
	info.pCommandBufferInfos = cmd;

	info.signalSemaphoreInfoCount = signal_semaphore == nullptr ? 0 : 1;
	info.pSignalSemaphoreInfos = signal_semaphore;

	return info;
}

vk::ImageCreateInfo image_create_info(const vk::Format format, vk::ImageUsageFlags usage_flags, vk::Extent3D extent)
{
	vk::ImageCreateInfo info = vk::ImageCreateInfo({},
												   vk::ImageType::e2D,
												   format,
												   extent,
												   1,
												   1,
												   vk::SampleCountFlagBits::e1,
												   vk::ImageTiling::eOptimal,
												   usage_flags
	);

	return info;
}

vk::ImageViewCreateInfo image_view_create_info(vk::Format format, vk::Image image, vk::ImageAspectFlags aspect_flags)
{
	vk::ImageViewCreateInfo info = vk::ImageViewCreateInfo(
		{},
		image,
		vk::ImageViewType::e2D,
		format
	);

	info.subresourceRange.baseMipLevel = 0;
	info.subresourceRange.levelCount = 1;
	info.subresourceRange.baseArrayLayer = 0;
	info.subresourceRange.layerCount = 1;
	info.subresourceRange.aspectMask = aspect_flags;

	return info;
}

vk::RenderingAttachmentInfo attachment_info(vk::ImageView view, vk::ClearValue* clear, vk::ImageLayout layout)
{
	vk::RenderingAttachmentInfo info;
	info.imageView = view;
	info.imageLayout = layout;
	info.loadOp = clear ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad;
	info.storeOp = vk::AttachmentStoreOp::eStore;
	
	if (clear != nullptr)
	{
		info.clearValue = *clear;
	}
	return info;
}

vk::RenderingInfo rendering_info(vk::Extent2D render_extent, vk::RenderingAttachmentInfo* color_attachment, vk::RenderingAttachmentInfo* depth_attachment)
{
	vk::RenderingInfo info;
	info.renderArea = vk::Rect2D{ vk::Offset2D{ 0, 0}, render_extent};
	info.layerCount = 1;
	info.colorAttachmentCount = 1;
	info.pColorAttachments = color_attachment;
	info.pDepthAttachment = depth_attachment;
	info.pStencilAttachment = nullptr;

	return info;
}

vk::PipelineLayoutCreateInfo pipeline_layout_create_info()
{
	vk::PipelineLayoutCreateInfo info;

	info.setLayoutCount = 0;
	info.pSetLayouts = nullptr;

	info.pushConstantRangeCount = 0;
	info.pSetLayouts = 0;
	info.pushConstantRangeCount = 0;
	info.pPushConstantRanges = nullptr;

	return info;
}

vk::BufferCreateInfo buffer_create_info(size_t size, vk::BufferUsageFlags usage)
{
	vk::BufferCreateInfo info({}, size, usage);

	return info;
}

vk::PipelineShaderStageCreateInfo pipeline_shader_stage_create_info(vk::ShaderStageFlagBits flags, vk::ShaderModule module)
{
	vk::PipelineShaderStageCreateInfo info({}, flags, module, "main");

	return info;
}

}