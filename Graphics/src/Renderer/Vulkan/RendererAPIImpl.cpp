#include "Engine.hpp"
#include "Renderer/RendererAPI.hpp"
#include "VulkanTexture.hpp"

#include <lud_assert.hpp>

namespace Ui::Renderer
{

using Detail::Vertex;

static bool s_initialized = false;

void Init(IWindow* window, bool use_imgui)
{
	Engine::Get().Init(window, use_imgui);
	s_initialized = true;
}

void Draw()
{
	Lud::check::eq(s_initialized, true, "Did you forgot to call to Renderer::Init ?");
	Engine::Get().Draw();
}

void Resize()
{
	Engine::Get().Resize();
}

void RequestResize()
{
	Engine::Get().Resize();
}


void DrawSprite(const Sprite& sprite)
{
	std::array<Vertex, 4> vertices;

	const float z_index = sprite.z_index;
	const auto [x, y, w, h] = sprite.rect;
	const auto [tx0, ty0, tx1, ty1] = sprite.texture_window;

	vertices[0].position = { x,     y,     z_index };
	vertices[1].position = { x + w, y,     z_index };
	vertices[2].position = { x + w, y + h, z_index };
	vertices[3].position = { x,     y + h, z_index };

	vertices[0].tex_coords = { /*-1, -1*/ tx0, ty0 };
	vertices[1].tex_coords = { /*-1, -1*/ tx1, ty0 };
	vertices[2].tex_coords = { /*-1, -1*/ tx1, ty1 };
	vertices[3].tex_coords = { /*-1, -1*/ tx0, ty1 };

	vertices[0].color = { 1.0f, 0.0f, 1.0f, 1.0f };
	vertices[1].color = { 1.0f, 1.0f, 0.0f, 1.0f };
	vertices[2].color = { 1.0f, 0.0f, 1.0f, 1.0f };
	vertices[3].color = { 0.0f, 1.0f, 1.0f, 1.0f };

	Engine::Get().SubmitDrawRect(vertices);
}

}