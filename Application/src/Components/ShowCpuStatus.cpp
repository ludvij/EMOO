#include "ShowCpuStatus.hpp"

#include "Application.hpp"


#include <cppicons/IconsFontAwesome5.hpp>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <ranges>

#include <embedding/explanations.hpp>


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
		if (ImGui::BeginChild("status", {}, ImGuiChildFlags_AutoResizeY))
		{
			draw_status();
		}
		ImGui::EndChild();
		ImGui::BeginGroup();
		auto& style = ImGui::GetStyle();
		const auto pc_text = "Go to PC";
		const float button_size = ImGui::CalcTextSize(pc_text).x + style.FramePadding.x * 2;
		if (ImGui::BeginChild("disassembly", { ImGui::GetContentRegionAvail().x * .7f,0 }))
		{
			ImGui::BeginChild("separator_text", ImVec2(ImGui::GetContentRegionAvail().x - button_size - style.ItemSpacing.x, 0), ImGuiChildFlags_AutoResizeY);
			ImGui::SeparatorText("Disassembly");
			ImGui::EndChild();
			ImGui::SameLine();
			if (ImGui::Button(pc_text))
			{
				m_track = true;
			}
			draw_assembly();
		}
		ImGui::EndChild();
		ImGui::SameLine();
		if (ImGui::BeginChild("stack", {}))
		{
			ImGui::SeparatorText("Stack");
			draw_stack();
		}
		ImGui::EndChild();
		ImGui::EndGroup();
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
		if (Application::Get().GetConsole().CanRun())
		{
			m_disassembler.Init();
			m_disassembler_initialised = true;
		}
		else
		{
			return;
		}
	}
	auto& cpu = Application::Get().GetConsole().GetCpu();
	auto& bus = Application::Get().GetConsole().GetBus();

	for (size_t i = 0; i < 256; i++)
	{
		m_stack[i] = bus.Peek(static_cast<u16>( 0x01FF - i ));
	}

	m_S = cpu.S();
	m_P = cpu.P();
	m_A = cpu.A();
	m_X = cpu.X();
	m_Y = cpu.Y();
	m_last_pc = m_PC;
	m_PC = cpu.PC();

	if (Application::Get().GetConsole().CanRun())
	{
		// just to disassemble in case it's not
		m_disassembler.Get(m_PC);
	}

}

void Ui::Component::ShowCPUStatus::OnCreate()
{
	m_disassembler.ConnectBus(&Application::Get().GetConsole().GetBus());
	if (Application::Get().GetConsole().CanRun())
	{
		m_disassembler.Init();
		m_disassembler_initialised = true;
	}
}

void Ui::Component::ShowCPUStatus::draw_status()
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
	}
	ImGui::EndGroup();
	ImGui::SameLine();
	ImGui::BeginGroup();
	{
		ImGui::Text("X register  %03d (%02X)", m_X, m_X);
		ImGui::Text("Y register  %03d (%02X)", m_Y, m_Y);
		ImGui::Text("A register  %03d (%02X)", m_A, m_A);
	}
	ImGui::EndGroup();
	ImGui::SameLine(ImGui::GetContentRegionAvail().x * .7f);
	ImGui::Text("Stack ptr  %03d (%02X)", m_S, m_S);
}

void Ui::Component::ShowCPUStatus::draw_stack()
{
	const auto& bus = Application::Get()
		.GetConsole()
		.GetBus();
	const u8 current_stack_position = m_S;
	const u16 stack_vector = Emu::CPU::STACK_VECTOR;
	std::vector<u8> stack_values;
	stack_values.reserve(0xFF - current_stack_position);
	// could be in the same loop, but whatever
	for (int i = current_stack_position; i <= 0xFF; i++)
	{
		stack_values.push_back(bus.Read(static_cast<u16>( stack_vector + i )));
	}
	u16 c = current_stack_position;
	for (const auto& sp : stack_values)
	{
		ImGui::Text("$%04X %02X", stack_vector + c++, sp);
	}
}

void Ui::Component::ShowCPUStatus::draw_assembly()
{

	const auto& assembly = m_disassembler.GetCache();



	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.16f, 0.17f, 0.2f, 1.0f));
	ImGui::BeginChild("scroll");
	for (const auto& [pc, repr] : assembly)
	{
		if (!repr.label.empty())
		{
			ImGui::Text(" %s:", repr.label.c_str());
		}
		if (m_PC == pc)
		{
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.22f, 0.24f, 0.29f, 1.0f));
			ImGui::BeginChild("tracked", {}, ImGuiChildFlags_AutoResizeY);
			ImGui::Text(" " ICON_FA_ARROW_RIGHT " $%04X %s", pc, repr.repr.c_str());
			ImGui::EndChild();
			ImGui::PopStyleColor();
			if (m_last_pc != m_PC || m_track)
			{
				m_track = false;
				// scroll to current pc
				ImGui::SetScrollHereY(.5f);
			}
		}
		else
		{
			ImGui::Text("   $%04X %s", pc, repr.repr.c_str());
		}
		if (ImGui::BeginItemTooltip())
		{
			ImGui::Text("%s - %s", map_title.at(repr.instruction), map_title.at(repr.addressing));
			ImGui::Dummy(ImVec2(400, 0));
			ImGui::Separator();
			ImGui::Indent();
			if (repr.size > 1)
			{
				ImGui::TextWrapped(map_explanation.at(repr.instruction), map_explanation.at(repr.addressing));
			}
			else
			{
				ImGui::TextWrapped(map_explanation.at(repr.instruction));
			}
			ImGui::Unindent();
			ImGui::EndTooltip();
		}
	}
	ImGui::EndChild();
	ImGui::PopStyleColor();
}
