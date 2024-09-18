#ifndef GRAPHICS_COMPONENT_SHOW_CPU_STATUS_HEADER
#define GRAPHICS_COMPONENT_SHOW_CPU_STATUS_HEADER

#include "IComponent.hpp"
#include <utils/Disassembler.hpp>

struct ImFont;

namespace Ui::Component
{
class ShowCPUStatus : public IComponent
{
public:
	ShowCPUStatus(const std::string_view name, ImFont* monospace);

	// Inherited via IComponent
	virtual void OnRender() override;
	virtual void OnUpdate() override;
	virtual void OnCreate() override;

private:
	void draw_status();

	void draw_stack();

	void draw_assembly();


private:
	uint8_t m_A{ 0 };
	uint8_t m_X{ 0 };
	uint8_t m_Y{ 0 };
	uint8_t m_P{ 0 };
	uint16_t m_PC{ 0 };
	uint8_t m_S{ 0 };
	uint16_t m_last_pc{ 0 };
	std::array<uint8_t, 256> m_stack{};
	A6502::Disassembler m_disassembler;
	bool m_track{ false };

	std::map<u16, A6502::Disassembly> m_current_pos;

	bool m_open{ true };
	bool m_disassembler_initialised{ false };

	ImFont* m_monospace;
};
}
#endif