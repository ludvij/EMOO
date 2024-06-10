#ifndef GRAPHICS_COMPONENT_SHOW_PPU_STATUS_HEADER
#define GRAPHICS_COMPONENT_SHOW_PPU_STATUS_HEADER

#include "IComponent.hpp"
#include <RendererAPI.hpp>


#include <array>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <NesEmu.hpp>

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
	void draw_oam();
	void draw_registers();

	void image_tooltip(uint32_t pat, ImVec2 pos, float size) const;

private:
	std::array<Emu::ObjectAttributeEntry, 64> m_oam;

	std::array<bool, 64> m_is_tracked{ false };

	char m_current_track[3];

	Renderer::ITexture* m_pattern_table[2]{ nullptr, nullptr };

	Renderer::ITexture* m_palette[8]{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	ImFont* m_monospace{ nullptr };

	uint8_t m_status{ 0 };
	uint8_t m_control{ 0 };
	uint8_t m_mask{ 0 };
	uint8_t m_latch{ 0 };
	uint16_t m_temp{ 0 };
	uint16_t m_addr{ 0 };
	uint8_t m_finex{ 0 };

	int m_bg_pal{ 4 };
	int m_sprite_pal{ 0 };

	bool m_update_chosen_pal{ false };

	bool m_open{ true };


	int m_cycles{};
	int m_scanlines{};
	int m_frames{};
	float m_frame_time{};
	float m_time_since_last{};

	bool m_first_popup_frame{ true };

};
}

#endif // !GRAPHICS_COMPONENT_SHOW_PPU_STATUS_HEADER
