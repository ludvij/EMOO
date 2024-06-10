#include "ShowCpuStatus.hpp"

#include "Application.hpp"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <ranges>

Ui::Component::ShowCPUStatus::ShowCPUStatus(const std::string_view name, ImFont* monospace)
	: IComponent(name)
	, m_monospace(monospace)
{

}

void Ui::Component::ShowCPUStatus::OnRender()
{
	const auto sp = ImGui::GetStyle().ItemSpacing;
	if (ImGui::Begin("CPU Status", &m_open))
	{
		ImGui::PushFont(m_monospace);
		const auto min = ImGui::GetWindowContentRegionMin();
		const auto pos = ImGui::GetWindowPos();
		const auto siz = ImGui::GetWindowSize();
		const auto max = ImGui::GetWindowContentRegionMax();
		const auto size = { max.x - min.x, max.y - min.y };
		ImGui::SeparatorText("Registers");
		if (ImGui::BeginChild("status", {}, ImGuiChildFlags_ResizeY))
		{

			ImGui::BeginGroup();
			{
				ImGui::Text("PS"); ImGui::SameLine(40);
				bool n = m_P & 0x80;
				bool v = m_P & 0x40;
				bool _ = m_P & 0x20;
				bool b = m_P & 0x10;
				bool d = m_P & 0x08;
				bool i = m_P & 0x04;
				bool z = m_P & 0x02;
				bool c = m_P & 0x01;
				ImGui::TextColored({ !n * 1.0f, n * 1.0f, 0.0f, 1.0f }, "N"); ImGui::SameLine();
				ImGui::TextColored({ !v * 1.0f, v * 1.0f, 0.0f, 1.0f }, "V"); ImGui::SameLine();
				ImGui::TextColored({ !_ * 1.0f, _ * 1.0f, 0.0f, 1.0f }, "-"); ImGui::SameLine();
				ImGui::TextColored({ !b * 1.0f, b * 1.0f, 0.0f, 1.0f }, "B"); ImGui::SameLine();
				ImGui::TextColored({ !d * 1.0f, d * 1.0f, 0.0f, 1.0f }, "D"); ImGui::SameLine();
				ImGui::TextColored({ !i * 1.0f, i * 1.0f, 0.0f, 1.0f }, "I"); ImGui::SameLine();
				ImGui::TextColored({ !z * 1.0f, z * 1.0f, 0.0f, 1.0f }, "Z"); ImGui::SameLine();
				ImGui::TextColored({ !c * 1.0f, c * 1.0f, 0.0f, 1.0f }, "C");
				//ImGui::BeginDisabled();
				//ImGui::Checkbox("Negative",    &n);
				//ImGui::Checkbox("Overflow",    &v);
				//ImGui::Checkbox("Unused",      &_);
				//ImGui::Checkbox("Break",       &b);
				//ImGui::Checkbox("Decimal",     &d);
				//ImGui::Checkbox("IRQ disable", &i);
				//ImGui::Checkbox("Zero",        &z);
				//ImGui::Checkbox("Carry",       &c);
				//ImGui::EndDisabled();
				ImGui::EndGroup();
			}
		}
		ImGui::EndChild();

		ImGui::PopFont();
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
	auto& cpu = Application::GetConsole().GetCpu();
	auto& bus = Application::GetConsole().GetBus();

	for (size_t i = 0; i < 256; i++)
	{
		m_stack[i] = bus.Peek(static_cast<u16>( 0x01FF - i ));
	}

	m_S = cpu.S();
	m_P = cpu.P();
	m_A = cpu.A();
	m_X = cpu.X();
	m_Y = cpu.Y();
	m_PC = cpu.PC();

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

void Ui::Component::ShowCPUStatus::draw_status()
{

}
