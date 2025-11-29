#include "Engine.hpp"
#include "RendererAPI.hpp"
#include "VulkanTexture.hpp"

#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

#include <ludutils/lud_assert.hpp>

namespace Renderer
{

using Detail::Vertex;

static bool s_initialized = false;

void Init(std::shared_ptr<Window::IWindow> window, bool use_imgui)
{
	Engine::Get().Init(window, use_imgui);
	s_initialized = true;
}

void Shutdown()
{
	Lud::assert::eq(s_initialized, true, "Did you forgot to call to Renderer::Init ?");
	Engine::Get().Cleanup();
	s_initialized = false;
}

void Draw()
{
	Lud::check::eq(s_initialized, true, "Did you forgot to call to Renderer::Init ?");
	Engine::Get().Draw();
}

void Resize()
{
	Lud::assert::eq(s_initialized, true, "Did you forgot to call to Renderer::Init ?");
	Engine::Get().Resize();
}


// TODO: make it work with normal textures too
void DrawSprite(const Sprite& sprite)
{
	Lud::assert::eq(s_initialized, true, "Did you forgot to call to Renderer::Init ?");
	std::array<Vertex, 4> vertices;

	const float z_index = sprite.z_index;
	const auto [x, y, w, h] = sprite.rect;
	const auto [tx0, ty0, tx1, ty1] = sprite.texture_window;

	vertices[0].position = { x,     y,     z_index };
	vertices[1].position = { x + w, y,     z_index };
	vertices[2].position = { x + w, y + h, z_index };
	vertices[3].position = { x,     y + h, z_index };

	vertices[0].tex_coords = { tx0, ty0 };
	vertices[1].tex_coords = { tx1, ty0 };
	vertices[2].tex_coords = { tx1, ty1 };
	vertices[3].tex_coords = { tx0, ty1 };

	if (sprite.texture)
	{
		VulkanBindlessTexture* tex = std::bit_cast<VulkanBindlessTexture*>( sprite.texture );
		vertices[0].texture_id = tex->id;
		vertices[1].texture_id = tex->id;
		vertices[2].texture_id = tex->id;
		vertices[3].texture_id = tex->id;
	}

	Engine::Get().SubmitDrawRect(vertices);
}

ITexture* CreateTexture(uint32_t w, uint32_t h, TextureType type)
{
	Lud::assert::eq(s_initialized, true, "Did you forgot to call to Renderer::Init ?");
	switch (type)
	{
	case Renderer::TextureType::NORMAL: return new VulkanTexture(w, h);
	case Renderer::TextureType::BINDLESS: return new VulkanBindlessTexture(w, h);
	default: return nullptr;
	}
}

ITexture* CreateTexture(uint32_t w, uint32_t h, void* data, TextureType type)
{
	Lud::assert::eq(s_initialized, true, "Did you forgot to call to Renderer::Init ?");
	switch (type)
	{
	case Renderer::TextureType::NORMAL: return new VulkanTexture(w, h, data);
	case Renderer::TextureType::BINDLESS: return new VulkanBindlessTexture(w, h, data);
	default: return nullptr;
	}
}

void BeginImGuiFrame()
{
	ImGui_ImplVulkan_NewFrame();
}

void BuildFontTexture()
{
	Engine::Get().ImmediateSubmit([&](vk::CommandBuffer cmd)
		{
			ImGui_ImplVulkan_CreateFontsTexture();
			
		});
}

}