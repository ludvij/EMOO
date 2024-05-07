#include "Engine.hpp"
#include "VulkanTexture.hpp"
#include <print>
namespace Ui
{


VulkanTexture::VulkanTexture(uint32_t w, uint32_t h)
	: ITexture(w, h)
{
	image = Engine::Get().CreateImage({ w, h, 1 }, vk::Format::eR8G8B8A8Unorm,
		vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst);
	Engine::Get().AddTextureToBatcher(this);
}

VulkanTexture::VulkanTexture(uint32_t w, uint32_t h, void* data)
	: ITexture(w, h)
{
	image = Engine::Get().CreateImage(data, { w, h, 1 }, vk::Format::eR8G8B8A8Unorm,
		vk::ImageUsageFlagBits::eSampled);
	Engine::Get().AddTextureToBatcher(this);
}

VulkanTexture::~VulkanTexture()
{
	Engine::Get().RemoveTextureFromBatcher(this);
	Engine::Get().DestroyImage(image);
}

void VulkanTexture::SetData(void* data)
{
	Engine::Get().SetImageData(image, data);
}

}
