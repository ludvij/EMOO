#include "ShowCpuStatus.hpp"

#include "Application.hpp"

#include <imgui.h>

#include <ranges>

Ui::Component::ShowCPUStatus::ShowCPUStatus(const std::string_view name)
	: IComponent(name)
{

}

void Ui::Component::ShowCPUStatus::OnRender()
{

	if (ImGui::Begin("Disassembly", &m_open))
	{
		for (const auto& [a, d] : m_current_pos)
		{
			if (!d.label.empty())
			{
				ImGui::Text("%s:", d.label.c_str());
			}
			ImGui::Text("  $%04X %s", a, d.repr.c_str());
		}
	}
	ImGui::End();

}

void Ui::Component::ShowCPUStatus::OnUpdate()
{

	if (!m_open)
	{
		removed = true;
		return;
	}
	if (!m_disassembler_initialised)
	{
		if (Application::GetConsole().CanRun())
		{
			m_disassembler.Init();
			m_disassembler_initialised = true;
		}
		else
		{
			return;
		}
	}
	const u16 pc = Application::GetConsole().GetCpu().PC();
	// add if not cached
	m_disassembler.Get(pc);


	const auto& disassembly = m_disassembler.GetCache();
	m_current_pos.clear();
	u16 last = pc;
	int count = 0;
	m_current_pos.clear();

	for (auto it = disassembly.upper_bound(pc); it != disassembly.end() && count < 10; ++it)
	{
		auto& elem = *it;
		// there is a gap
		if (elem.first - last > 3)
		{
			break;
		}
		count++;
		m_current_pos.insert(elem);
		last = elem.first;
	}

}

void Ui::Component::ShowCPUStatus::OnCreate()
{
	m_disassembler.ConnectBus(&Application::GetConsole().GetBus());
	if (Application::GetConsole().CanRun())
	{
		m_disassembler.Init();
		m_disassembler_initialised = true;
	}
}
