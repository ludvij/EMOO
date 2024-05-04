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
	Sprite() = default;
	virtual ~Sprite() = default;
	Sprite(Rect pos, float z_index, uint32_t slot = 0, TextureWindow texture_window = { 0.0f, 0.0f, 1.0f, 1.0f });
	//void Move(const Rect new_pos);
	//void Apply(glm::vec2 direction);

	//void Update();
	void Draw() const;

public: // Who cares anymore
	float z_index{ 0 };
	Rect rect{ 0, 0, 0, 0 };
	TextureWindow texture_window{ 0.0f, 0.0f, 1.0f, 1.0f };
	uint32_t slot{ 0 };
};
}
#endif//!GRAPHICS_SPRITE_HEADER