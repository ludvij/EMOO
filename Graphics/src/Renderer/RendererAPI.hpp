#ifndef GRAPHICS_RENDERER_API_HEADER
#define GRAPHICS_RENDERER_API_HEADER

#include "Window/Window.hpp"

#include "Sprite.hpp"
#include "Texture.hpp"


namespace Ui::Renderer
{
void Init(IWindow* window, bool use_imgui=true);
void Draw();
void Resize();
void RequestResize();

void DrawSprite(const Sprite& sprite);
}
#endif //! GRAPHICS_RENDERER_API_HEADER