#ifndef GRAPHICS_COMPONENT_SHOW_CPU_STATUS_HEADER
#define GRAPHICS_COMPONENT_SHOW_CPU_STATUS_HEADER

#include "IComponent.hpp"
#include <utils/Disassembler.hpp>

namespace Ui::Component
{
class ShowCPUStatus : public IComponent
{
public:
	ShowCPUStatus(const std::string_view name);

	// Inherited via IComponent
	virtual void OnRender() override;
	virtual void OnUpdate() override;
	virtual void OnCreate() override;

private:
	A6502::Disassembler m_disassembler;

	std::map<u16, A6502::Disassembly> m_current_pos;

	bool m_open{ true };
	bool m_disassembler_initialised{ false };
};
}
#endif