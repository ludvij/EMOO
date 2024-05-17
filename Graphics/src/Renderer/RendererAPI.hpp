#ifndef GRAPHICS_RENDERER_API_HEADER
#define GRAPHICS_RENDERER_API_HEADER

#include "Window/Window.hpp"

#include "Sprite.hpp"
#include "Texture.hpp"

#include <imgui.h>
#include <memory>



namespace Ui::Renderer
{
void Init(std::shared_ptr<IWindow> window, bool use_imgui=true);
void Shutdown();
void Draw();
void Resize();
void RequestResize();

void DrawSprite(const Sprite& sprite);

// user needs to delete texture manually
ITexture* CreateBindlessTexture(uint32_t w, uint32_t h);
// user needs to delete texture manually
ITexture* CreateBindlessTexture(uint32_t w, uint32_t h, void* data);

ITexture* CreateImGuiTexture(uint32_t w, uint32_t h);
ITexture* CreateImGuiTexture(uint32_t w, uint32_t h, void* data);

ImTextureID TextureAsImgui(ITexture* texture);

void BeginImGuiFrame();
}
#endif //! GRAPHICS_RENDERER_API_HEADER