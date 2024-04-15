#include "Images.hpp"


namespace Ui::vkutil
{
void transition_image(vk::CommandBuffer cmd, vk::Image image, vk::ImageLayout current_layout, vk::ImageLayout new_layout)
{
	vk::ImageMemoryBarrier2 image_barrier(
		vk::PipelineStageFlagBits2::eAllCommands,
		vk::AccessFlagBits2::eMemoryWrite,
		vk::PipelineStageFlagBits2::eAllCommands,
		vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead,
		current_layout,
		new_layout
	);

	vk::ImageAspectFlagBits aspect_mask = (new_layout == vk::ImageLayout::eDepthAttachmentOptimal) ?
		vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor;
	image_barrier.subresourceRange = vkinit::image_subresource_range(aspect_mask);
	image_barrier.image = image;

	vk::DependencyInfo dep_info;
	dep_info.imageMemoryBarrierCount = 1;
	dep_info.pImageMemoryBarriers = &image_barrier;

	cmd.pipelineBarrier2(dep_info);
}
void copy_image_to_image(vk::CommandBuffer cmd, vk::Image src, vk::Image dst, vk::Extent2D src_size, vk::Extent2D dst_size)
{
	vk::ImageBlit2 blit_region;

	blit_region.srcOffsets[1].x = src_size.width;
	blit_region.srcOffsets[1].y = src_size.height;
	blit_region.srcOffsets[1].z = 1;

	blit_region.dstOffsets[1].x = dst_size.width;
	blit_region.dstOffsets[1].y = dst_size.height;
	blit_region.dstOffsets[1].z = 1;

	blit_region.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	blit_region.srcSubresource.baseArrayLayer = 0;
	blit_region.srcSubresource.layerCount = 1;
	blit_region.srcSubresource.mipLevel = 0;

	blit_region.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	blit_region.dstSubresource.baseArrayLayer = 0;
	blit_region.dstSubresource.layerCount = 1;
	blit_region.dstSubresource.mipLevel = 0;

	vk::BlitImageInfo2 info(
		src, vk::ImageLayout::eTransferSrcOptimal,
		dst, vk::ImageLayout::eTransferDstOptimal,
		blit_region,
		vk::Filter::eLinear
	);
	cmd.blitImage2(info);
}
}