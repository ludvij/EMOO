#include "Texture.hpp"

#include <imgui.h>

#include <lud_assert.hpp>

ImTextureID Renderer::ITexture::ToImGui() const
{
	// not implemented
	Lud::assert::eq(this, nullptr, "Unimplemented");
	return nullptr;
}
