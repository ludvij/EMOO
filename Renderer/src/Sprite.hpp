#ifndef GRAPHICS_SPRITE_HEADER
#define GRAPHICS_SPRITE_HEADER
#include <glm/mat4x4.hpp>

#include "Primitives.hpp"
#include "Texture.hpp"

namespace Renderer
{
class Sprite
{
public:
	Sprite() = default;
	virtual ~Sprite() = default;
	Sprite(Rect pos, float z_index, ITexture* texture, TextureWindow texture_window = { 0.0f, 0.0f, 1.0f, 1.0f });
	//void Move(const Rect new_pos);
	//void Apply(glm::vec2 direction);
	void SetTexture(ITexture* texture, TextureWindow texture_window = { 0.0f, 0.0f, 1.0f, 1.0f });

	//void Update();
	void Draw() const;

public: // Who cares anymore
	Rect rect{ 0, 0, 0, 0 };
	float z_index{ 0 };
	ITexture* texture{ nullptr };
	TextureWindow texture_window{ 0.0f, 0.0f, 1.0f, 1.0f };
};
}
#endif//!GRAPHICS_SPRITE_HEADER