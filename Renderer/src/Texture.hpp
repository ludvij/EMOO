#ifndef GRAPHICS_RENDERER_TEXTURE_HEADER
#define GRAPHICS_RENDERER_TEXTURE_HEADER

#include <cstdint>
#include <imgui.h>

namespace Renderer
{

struct TextureWindow
{
	float x0{ 0.0f };
	float y0{ 0.0f };
	float x1{ 1.0f };
	float y1{ 1.0f };
};


enum class TextureType
{
	NORMAL,
	BINDLESS
};

class ITexture
{
public:
	ITexture(uint32_t w, uint32_t h)
		: m_w(w)
		, m_h(h)
	{
	};
	ITexture(uint32_t w, uint32_t h, void* data)
		: ITexture(w, h)
	{
	};
	virtual ~ITexture() = default;
	virtual void SetData(void* data) = 0;

	virtual ImTextureID ToImGui() const;

protected:
	uint32_t m_w;
	uint32_t m_h;
};
}
#endif //!GRAPHICS_RENDERER_TEXTURE_HEADER