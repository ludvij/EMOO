#include "Engine.hpp"
#include "VulkanTexture.hpp"

namespace Ui
{


VulkanTexture::VulkanTexture(uint32_t w, uint32_t h)
	: ITexture(w, h)
{
	m_image = Engine::Get().CreateImage({ w, h, 1 }, vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eSampled);
}

VulkanTexture::VulkanTexture(uint32_t w, uint32_t h, void* data)
	: ITexture(w, h)
{
	m_image = Engine::Get().CreateImage(data, { w, h, 1 }, vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eSampled);
}

VulkanTexture::~VulkanTexture()
{
	Engine::Get().DestroyImage(m_image);
}

void VulkanTexture::SetData(void* data)
{
	Engine::Get().SetImageData(m_image, data);
}

}
