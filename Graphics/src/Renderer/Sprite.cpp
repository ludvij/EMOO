#include "Sprite.hpp"

#include "RendererAPI.hpp"

namespace Ui
{
Sprite::Sprite(Rect pos, float z_index, uint32_t slot, TextureWindow texture_window)
	: rect(pos)
	, z_index(z_index)
	, slot(slot)
	, texture_window(texture_window)
{

}
void Sprite::Draw() const
{
	Renderer::DrawSprite(*this);
}
}