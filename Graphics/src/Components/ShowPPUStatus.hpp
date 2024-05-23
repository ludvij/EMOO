#ifndef GRAPHICS_COMPONENT_SHOW_PPU_STATUS_HEADER
#define GRAPHICS_COMPONENT_SHOW_PPU_STATUS_HEADER

#include "IComponent.hpp"
#include "Renderer/RendererAPI.hpp"
#include <vector>


namespace Ui::Component
{
class ShowPPUStatus : public IComponent
{
public:

	~ShowPPUStatus();
	virtual void OnCreate() override;
	// Inherited via IComponent
	virtual void OnRender() override;
	virtual void OnUpdate() override;

private:
	ITexture* m_pattern_table[2];

	ITexture* m_palette[8];

	int m_bg_pal{ 4 };
	int m_sprite_pal{ 0 };

	bool m_update_chosen_pal{ false };

	bool m_open{ true };

};
}

#endif // !GRAPHICS_COMPONENT_SHOW_PPU_STATUS_HEADER
