#include "Engine.hpp"
#include "VulkanTexture.hpp"

#include "backends/imgui_impl_vulkan.h"


#include <print>
namespace Renderer
{


VulkanBindlessTexture::VulkanBindlessTexture(uint32_t w, uint32_t h)
	: ITexture(w, h)
{
	image = Engine::Get().CreateImage({ w, h, 1 }, vk::Format::eR8G8B8A8Unorm,
		vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst);
	Engine::Get().AddTextureToBatcher(this);
}

VulkanBindlessTexture::VulkanBindlessTexture(uint32_t w, uint32_t h, void* data)
	: ITexture(w, h)
{
	image = Engine::Get().CreateImage(data, { w, h, 1 }, vk::Format::eR8G8B8A8Unorm,
		vk::ImageUsageFlagBits::eSampled);
	Engine::Get().AddTextureToBatcher(this);

}

VulkanBindlessTexture::~VulkanBindlessTexture()
{
	Engine::Get().RemoveTextureFromBatcher(this);
	Engine::Get().DestroyImage(image);
}

void VulkanBindlessTexture::SetData(void* data)
{
	Engine::Get().SetImageData(image, data);
}

ImTextureID VulkanTexture::ToImGui() const
{
	return static_cast<ImTextureID>( descriptor_set );
}

VulkanTexture::VulkanTexture(uint32_t w, uint32_t h)
	: ITexture(w, h)
{
	image = Engine::Get().CreateImage({ w, h, 1 }, vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst);
	// TODO: not use this please
	descriptor_set = ImGui_ImplVulkan_AddTexture(
		Engine::Get().GetSampler(),
		image.view,
		static_cast<VkImageLayout>( vk::ImageLayout::eShaderReadOnlyOptimal )
	);

	Engine::Get().ImmediateSubmit([&](vk::CommandBuffer cmd)
		{
			vkutil::transition_image(cmd, image.image, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);
		});

}

VulkanTexture::VulkanTexture(uint32_t w, uint32_t h, void* data)
	: ITexture(w, h)
{
	image = Engine::Get().CreateImage(data, { w, h, 1 }, vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eTransferDst);
	// TODO: not use this please
	descriptor_set = ImGui_ImplVulkan_AddTexture(
		Engine::Get().GetSampler(),
		image.view,
		static_cast<VkImageLayout>( vk::ImageLayout::eShaderReadOnlyOptimal )
	);

	Engine::Get().ImmediateSubmit([&](vk::CommandBuffer cmd)
		{
			vkutil::transition_image(cmd, image.image, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);
		});

}

VulkanTexture::~VulkanTexture()
{
	Engine::Get().GetDevice().waitIdle();

	Engine::Get().DestroyImage(image);
	ImGui_ImplVulkan_RemoveTexture(descriptor_set);
}

void VulkanTexture::SetData(void* data)
{
	Engine::Get().SetImageData(image, data);
}


}
