#ifndef RENDERER_VULKAN_TEXTURE_HEADER
#include "imgui.h"
#include "Texture.hpp"

#include "Core.hpp"

namespace Ui
{
class VulkanBindlessTexture : public ITexture
{
public:
	VulkanBindlessTexture(uint32_t w, uint32_t h);
	VulkanBindlessTexture(uint32_t w, uint32_t h, void* data);
	virtual ~VulkanBindlessTexture() override;

	virtual void SetData(void* data) override;

	Detail::AllocatedImage image;
	uint32_t id{ 0 };

private:
};


class VulkanImGuiTexture : public ITexture
{
public:
	VulkanImGuiTexture(uint32_t w, uint32_t h);
	VulkanImGuiTexture(uint32_t w, uint32_t h, void* data);
	virtual ~VulkanImGuiTexture() override;

	virtual void SetData(void* data) override;

	operator ImTextureID() const
	{
		return static_cast<ImTextureID>( descriptor_set );
	}

	ImTextureID AsImTexture() const
	{
		return static_cast<ImTextureID>( descriptor_set );
	}

	Detail::AllocatedImage image;
	vk::DescriptorSet descriptor_set;
};
}

#define RENDERER_VULKAN_TEXTURE_HEADER
#endif //!RENDERER_VULKAN_TEXTURE_HEADER