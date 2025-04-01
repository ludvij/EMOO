#include "Sprite.hpp"

#include "RendererAPI.hpp"

namespace Renderer
{
Sprite::Sprite(Rect pos, float z_index, ITexture* texture, TextureWindow texture_window)
	: rect(pos)
	, z_index(z_index)
	, texture(texture)
	, texture_window(texture_window)
{

}

void Sprite::SetTexture(ITexture* new_texture, TextureWindow new_texture_window)
{
	this->texture = new_texture;
	this->texture_window = new_texture_window;
}

void Sprite::Draw() const
{
	Renderer::DrawSprite(*this);
}
}