#ifndef RENDERER_VULKAN_TEXTURE_HEADER
#include "imgui.h"
#include "Texture.hpp"

#include "Core.hpp"

namespace Renderer
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


class VulkanTexture : public ITexture
{
public:
	VulkanTexture(uint32_t w, uint32_t h);
	VulkanTexture(uint32_t w, uint32_t h, void* data);
	virtual ~VulkanTexture() override;

	virtual void SetData(void* data) override;


	virtual ImTextureID ToImGui() const override;

	Detail::AllocatedImage image;
	vk::DescriptorSet descriptor_set;
};
}

#define RENDERER_VULKAN_TEXTURE_HEADER
#endif //!RENDERER_VULKAN_TEXTURE_HEADER