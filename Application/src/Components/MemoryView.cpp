#include "MemoryView.hpp"

#include "Application.hpp"
#include <imgui.h>
#include <ludutils/lud_parse.hpp>
#include <misc/cpp/imgui_stdlib.h>
#include <RendererAPI.hpp>

//https://pixeljoint.com/forum/forum_posts.asp?TID=12795
static constexpr std::array<u32, 16> dawnbringer16 = {
	DEFINE_COLOR(20, 12, 28),
	DEFINE_COLOR(68, 36, 52),
	DEFINE_COLOR(48, 52, 109),
	DEFINE_COLOR(78, 74, 78),
	DEFINE_COLOR(133, 76, 48),
	DEFINE_COLOR(52, 101, 36),
	DEFINE_COLOR(208, 70, 72),
	DEFINE_COLOR(117, 113, 97),
	DEFINE_COLOR(89, 125, 206),
	DEFINE_COLOR(210, 125, 44),
	DEFINE_COLOR(133, 149, 161),
	DEFINE_COLOR(109, 170, 44),
	DEFINE_COLOR(210, 170, 153),
	DEFINE_COLOR(109, 194, 202),
	DEFINE_COLOR(218, 212, 94),
	DEFINE_COLOR(222, 238, 214),
};

static void CreateMemoryTooltip(size_t addr);

Ui::Component::MemoryView::MemoryView(const std::string_view name, ImFont* font)
	: IComponent(name)
	, m_font(font)
{

}

void Ui::Component::MemoryView::OnCreate()
{
	m_memory     = Renderer::CreateTexture(128, 128);
	m_cpu_ram_2  = Renderer::CreateTexture(128, 128);
	m_cpu_ram_16 = Renderer::CreateTexture(64, 64);
}

Ui::Component::MemoryView::~MemoryView()
{
	delete m_memory;
	delete m_cpu_ram_2;
	delete m_cpu_ram_16;
}

void Ui::Component::MemoryView::OnRender()
{
	if (ImGui::Begin("Memory Viewer", &m_open))
	{
		ImGui::PushFont(m_font);
		const auto pad = ImGui::GetStyle().FramePadding;
		const auto sp = ImGui::GetStyle().ItemSpacing;
		const auto avail = ImGui::GetContentRegionAvail();


		const float size_x = ( avail.x - sp.x ) / 2.0f;

		ImGui::Text("Full memory range");
		ImGui::SameLine(pad.x + ( size_x + sp.x ) * 1);
		ImGui::Text("CPU RAM");
		ImGui::SameLine();
		const char* text = "set 4 bit per pixel";
		if (m_16)
		{
			text = "set 1 bit per pixel";
		}
		if (ImGui::SmallButton(text))
		{
			m_16 = !m_16;
		}

		ImGui::Image(m_memory->ToImGui(), { size_x, size_x }, { 0, 0 }, { 1, 1 }, { 1,1,1,1 }, { 1,1,1,1 });
		ImGui::SameLine(pad.x + ( size_x + sp.x ) * 1);
		if (m_16)
		{
			ImGui::Image(m_cpu_ram_16->ToImGui(), { size_x, size_x }, { 0, 0 }, { 1, 1 }, { 1,1,1,1 }, { 1,1,1,1 });
		}
		else
		{
			ImGui::Image(m_cpu_ram_2->ToImGui(), { size_x, size_x }, { 0, 0 }, { 1, 1 }, { 1,1,1,1 }, { 1,1,1,1 });
		}

		ImGuiWindowFlags w_flags =
			//ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_HorizontalScrollbar;
		// there has to be a better way
		const float table_x = 686;
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - table_x / 2);
		if (ImGui::BeginChild("text", {}, ImGuiChildFlags_AutoResizeX, w_flags))
		{
			if (ImGui::BeginTable("text repr", 0x10 + 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX))
			{
				auto s = ImGui::GetWindowSize();
				const float spacing = ImGui::GetTextLineHeightWithSpacing();
				const float amount = ( s.y / spacing - 1 ) * 0x10;
				for (size_t i = m_begin; i < m_begin + amount; i+= 0x10)
				{
					if (i > 0xFFFF) break;
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("$%04X", i);
					char ascii[17] = {};
					for (size_t j = 0; j < 0x10; j++)
					{
						auto c = m_memory_cache[i + j];
						ImGui::TableNextColumn();
						if (m_last == i + j)
						{

							float b = EaseInOutCubic(m_fadeout, fade_seconds, -fade_seconds);
							ImU32 color = ImColor(1.0f, 1.0f, b);
							ImGui::PushStyleColor(ImGuiCol_Text, color);
							ImGui::Text("%02X", c);
							ImGui::PopStyleColor();
						}
						else
						{
							ImGui::Text("%02X", c);
						}
						CreateMemoryTooltip(i + j);
						ascii[j] = isprint(c) ? c : '.';
					}
					ImGui::TableNextColumn();
					ImGui::Text("|%s|", ascii);
				}

				if (ImGui::IsWindowHovered())
				{
					auto& io = ImGui::GetIO();
					const int scroll = m_scroll_amount * 0x10;
					// scrolling
					if (io.MouseWheel > 0)
					{
						m_begin = m_begin >= scroll ? m_begin - scroll : 0x0000;
					}
					else if (io.MouseWheel < 0 && m_begin + scroll <= 0xFFFF)
					{
						m_begin += scroll;
					}
				}
				ImGui::EndTable();
			}


			if (ImGui::BeginPopupContextItem("Search"))
			{
				ImGui::Text("Go to:");
				ImGui::SameLine();
				ImGuiInputTextFlags flags =
					ImGuiInputTextFlags_EscapeClearsAll |
					ImGuiInputTextFlags_CharsUppercase |
					ImGuiInputTextFlags_CharsHexadecimal |
					ImGuiInputTextFlags_EnterReturnsTrue;
				if (m_first_popup_frame)
				{
					ImGui::SetKeyboardFocusHere();
					m_first_popup_frame = false;
				}
				if (ImGui::InputText("Address", m_text_buffer, IM_ARRAYSIZE(m_text_buffer), flags))
				{
					m_last = Lud::parse_num<u16>(m_text_buffer, 16);
					m_fadeout = fade_seconds;
					m_begin = static_cast<size_t>( m_last > 0x50 ? ( m_last & 0xFFF0 ) - 0x50 : 0x0000 );
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::Button("Go"))
				{
					m_last = Lud::parse_num<u16>(m_text_buffer, 16);
					m_fadeout = fade_seconds;
					m_begin = static_cast<size_t>( m_last > 0x50 ? ( m_last & 0xFFF0 ) - 0x50 : 0x0000 );
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Close"))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			else
			{
				std::fill_n(m_text_buffer, IM_ARRAYSIZE(m_text_buffer), 0);
				m_first_popup_frame = true;
			}
		}
		ImGui::EndChild();
		ImGui::PopFont();
	}
	ImGui::End();
}



void Ui::Component::MemoryView::OnUpdate()
{
	if (!m_open)
	{
		removed = true;
		return;
	}

	if (m_fadeout >= 0)
	{
		m_fadeout -= static_cast<float>( Application::Get().GetDelta() );
	}
	else
	{
		m_last = -1;
	}


	if (!Application::Get().GetConsole().CanRun())
	{
		return;
	}
	const auto& bus = Application::Get().GetConsole().GetBus();
	u32* buf = new u32[128 * 128];
	size_t count = 0;
	for (size_t i = 0; i <= 0xFFFF; i+= 4)
	{
		u8 R = bus.Peek(static_cast<u16>( i + 0 ));
		u8 G = bus.Peek(static_cast<u16>( i + 1 ));
		u8 B = bus.Peek(static_cast<u16>( i + 2 ));
		u8 A = bus.Peek(static_cast<u16>( i + 3 ));

		buf[count++] = DEFINE_COLOR_A(R, G, B, A);

		m_memory_cache[i + 0] = R;
		m_memory_cache[i + 1] = G;
		m_memory_cache[i + 2] = B;
		m_memory_cache[i + 3] = A;
	}
	m_memory->SetData(buf);
	if (m_16)
	{
		repr_cpu_ram_16();
	}
	else
	{
		repr_cpu_ram_2();
	}

	delete[] buf;
}

void Ui::Component::MemoryView::repr_cpu_ram_16()
{
	const auto& bus = Application::Get().GetConsole().GetBus();
	u32* buf = new u32[64 * 64];

	size_t count = 0;
	for (size_t i = 0; i < 0x800; i++)
	{
		u8 dat = bus.Peek(static_cast<u16>(i));
		u8 lo = dat & 0x0F;
		u8 hi = ( dat & 0xF0 ) >> 4;
		buf[count++] = dawnbringer16[lo];
		buf[count++] = dawnbringer16[hi];
	}
	m_cpu_ram_16->SetData(buf);
	delete[] buf;

}

void Ui::Component::MemoryView::repr_cpu_ram_2()
{
	const auto& bus = Application::Get().GetConsole().GetBus();
	u32* buf = new u32[128 * 128];

	size_t count = 0;
	for (size_t i = 0; i < 0x800; i++)
	{
		u8 dat = bus.Peek(static_cast<u16>(i));
		buf[count++] = dat & 0x01 ? DEFINE_COLOR(0xFF, 0xFF, 0xFF) : DEFINE_COLOR(0x00, 0x00, 0x00);
		buf[count++] = dat & 0x02 ? DEFINE_COLOR(0xFF, 0xFF, 0xFF) : DEFINE_COLOR(0x00, 0x00, 0x00);
		buf[count++] = dat & 0x04 ? DEFINE_COLOR(0xFF, 0xFF, 0xFF) : DEFINE_COLOR(0x00, 0x00, 0x00);
		buf[count++] = dat & 0x08 ? DEFINE_COLOR(0xFF, 0xFF, 0xFF) : DEFINE_COLOR(0x00, 0x00, 0x00);
		buf[count++] = dat & 0x10 ? DEFINE_COLOR(0xFF, 0xFF, 0xFF) : DEFINE_COLOR(0x00, 0x00, 0x00);
		buf[count++] = dat & 0x20 ? DEFINE_COLOR(0xFF, 0xFF, 0xFF) : DEFINE_COLOR(0x00, 0x00, 0x00);
		buf[count++] = dat & 0x40 ? DEFINE_COLOR(0xFF, 0xFF, 0xFF) : DEFINE_COLOR(0x00, 0x00, 0x00);
		buf[count++] = dat & 0x80 ? DEFINE_COLOR(0xFF, 0xFF, 0xFF) : DEFINE_COLOR(0x00, 0x00, 0x00);
	}
	m_cpu_ram_2->SetData(buf);
	delete[] buf;

}

void CreateMemoryTooltip(size_t addr)
{
	std::string text = std::format("Address: ${:04X}", addr);
	if (addr < 0x100)
	{
		text += "\nMemory sector: CPU RAM Zero page";
	}
	else if (addr < 0x0800)
	{
		text += "\nMemory sector: CPU RAM";
	}
	else if (addr < 0x2000)
	{
		text += "\nMemory sector: CPU RAM mirrors";
	}
	else if (addr < 0x2008)
	{
		text += "\nMemory sector: PPU registers";
	}
	else if (addr < 0x4000)
	{
		text += "\nMemory sector: PPU registers mirrors";
	}
	else if (addr < 0x4018)
	{
		text += "\nMemory sector: APU & IO registers";
	}
	else if (addr < 0x4020)
	{
		text += "\nMemory sector: Test mode";
	}
	else if (addr < 0x6000)
	{
		text += "\nMemory sector: Cartridge space";
	}
	else if (addr < 0x8000)
	{
		text += "\nMemory sector: Cartridge RAM";
	}
	else
	{
		text += "\nMemory sector: Cartridge ROM & mapper registers";
	}
	ImGui::SetItemTooltip(text.c_str());
}
