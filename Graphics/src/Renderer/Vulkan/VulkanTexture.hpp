#ifndef RENDERER_VULKAN_TEXTURE_HEADER
#include "Renderer/Texture.hpp"

#include "Core.hpp"

namespace Ui
{
class VulkanTexture : public ITexture
{
public:
	VulkanTexture(uint32_t w, uint32_t h);
	VulkanTexture(uint32_t w, uint32_t h, void* data);
	virtual ~VulkanTexture() override;

	virtual void SetData(void* data) override;

private:
	Detail::AllocatedImage m_image;
};
}

#define RENDERER_VULKAN_TEXTURE_HEADER
#endif //!RENDERER_VULKAN_TEXTURE_HEADER