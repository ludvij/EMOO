#ifndef GRAPHICS_RENDERER_API_HEADER
#define GRAPHICS_RENDERER_API_HEADER

#include <Window/Window.hpp>

#include "Sprite.hpp"
#include "Texture.hpp"

#include <imgui.h>
#include <memory>

struct ImFont;

namespace Renderer
{
void Init(std::shared_ptr<Window::IWindow> window, bool use_imgui=true);
void Shutdown();
void Draw();
void Resize();
void RequestResize();

void DrawSprite(const Sprite& sprite);

// user needs to delete texture manually
ITexture* CreateTexture(uint32_t w, uint32_t h, TextureType type=TextureType::NORMAL);
// user needs to delete texture manually
ITexture* CreateTexture(uint32_t w, uint32_t h, void* data, TextureType type=TextureType::NORMAL);

ImFont* GetMonospaceFont();

void BeginImGuiFrame();
}
#endif //! GRAPHICS_RENDERER_API_HEADER