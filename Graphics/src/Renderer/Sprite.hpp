#ifndef GRAPHICS_SPRITE_HEADER
#define GRAPHICS_SPRITE_HEADER
#include <glm/mat4x4.hpp>

#include "Primitives.hpp"
#include "Texture.hpp"

namespace Ui
{
class Sprite
{
public:
	Sprite(Rect pos, float z_index, ITexture* texture = nullptr, TextureWindow texture_window = { 0.0f, 0.0f, 1.0f, 1.0f });
	//void Move(const Rect new_pos);
	//void Apply(glm::vec2 direction);

	//void Update();
	void Draw() const;
public:
	float z_index;
	Rect rect;
	TextureWindow texture_window;
private:
	ITexture* m_texture;
};
}
#endif//!GRAPHICS_SPRITE_HEADER