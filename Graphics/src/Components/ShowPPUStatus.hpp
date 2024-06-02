#ifndef GRAPHICS_COMPONENT_SHOW_PPU_STATUS_HEADER
#define GRAPHICS_COMPONENT_SHOW_PPU_STATUS_HEADER

#include "IComponent.hpp"
#include "Renderer/RendererAPI.hpp"

#include <string_view>
#include <vector>

struct ImFont;

namespace Ui::Component
{
class ShowPPUStatus : public IComponent
{
public:
	ShowPPUStatus(const std::string_view name, ImFont* monospace);
	virtual ~ShowPPUStatus() override;
	virtual void OnCreate() override;
	// Inherited via IComponent
	virtual void OnRender() override;
	virtual void OnUpdate() override;

private:
	void draw_images();

private:
	ITexture* m_pattern_table[2]{ nullptr, nullptr };

	ITexture* m_palette[8]{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	uint8_t m_status{ 0 };
	uint8_t m_control{ 0 };
	uint8_t m_mask{ 0 };

	int m_bg_pal{ 4 };
	int m_sprite_pal{ 0 };

	bool m_update_chosen_pal{ false };

	bool m_open{ true };

	ImFont* m_monospace;

	int m_cycles;
	int m_scanlines;

};
}

#endif // !GRAPHICS_COMPONENT_SHOW_PPU_STATUS_HEADER
