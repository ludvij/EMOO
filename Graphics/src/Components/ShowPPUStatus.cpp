#include "Renderer/RendererAPI.hpp"
#include "ShowPPUStatus.hpp"

#include "Application.hpp"

#include <imgui.h>
#include <lud_parser.hpp>
#include <misc/cpp/imgui_stdlib.h>

Ui::Component::ShowPPUStatus::ShowPPUStatus(const std::string_view name, ImFont* font)
	: IComponent(name)
	, m_monospace(font)
{
}

Ui::Component::ShowPPUStatus::~ShowPPUStatus()
{
	for (size_t i = 0; i < 8; i++)
	{
		delete m_palette[i];
	}

	for (size_t i = 0; i < 2; i++)
	{
		delete m_pattern_table[i];
	}

}

void Ui::Component::ShowPPUStatus::OnCreate()
{
	m_pattern_table[0] = Renderer::CreateImGuiTexture(128, 128);
	m_pattern_table[1] = Renderer::CreateImGuiTexture(128, 128);

	m_palette[0] = Renderer::CreateImGuiTexture(4, 1);
	m_palette[1] = Renderer::CreateImGuiTexture(4, 1);
	m_palette[2] = Renderer::CreateImGuiTexture(4, 1);
	m_palette[3] = Renderer::CreateImGuiTexture(4, 1);

	m_palette[4] = Renderer::CreateImGuiTexture(4, 1);
	m_palette[5] = Renderer::CreateImGuiTexture(4, 1);
	m_palette[6] = Renderer::CreateImGuiTexture(4, 1);
	m_palette[7] = Renderer::CreateImGuiTexture(4, 1);
}

void Ui::Component::ShowPPUStatus::OnRender()
{
	if (ImGui::Begin("PPU status", &m_open))
	{
		ImGui::PushFont(m_monospace);
		draw_images();
		ImGui::SeparatorText("Cycles");
		ImGui::Text("Cycle:    %d", m_cycles);
		ImGui::Text("Scanline: %d", m_scanlines);
		ImGui::Text("Frames:   %d", m_frames);

		draw_registers();
		ImGui::SeparatorText("OAM");
		draw_oam();
		ImGui::PopFont();
	}
	ImGui::End();
	//ImGui::PopStyleVar();
}


void Ui::Component::ShowPPUStatus::OnUpdate()
{
	if (!m_open)
	{
		removed = true;
		return;
	}

	auto& ppu = Application::GetConsole().GetPpu();
	auto& bus = Application::GetConsole().GetBus();
	m_control = bus.Peek(0x2000);
	m_mask    = bus.Peek(0x2001);
	m_status  = bus.Peek(0x2002);
	m_cycles  = ppu.GetCycles();
	m_frames  = ppu.GetFrames();
	m_latch = ppu.W();
	m_temp = ppu.T();
	m_addr = ppu.V();
	m_finex = ppu.X();
	m_scanlines = ppu.GetScanlines();
	for (size_t i = 0; i < 64; i++)
	{
		m_oam[i] = ppu.GetOAMEntry(i);
	}
	if (ppu.HasUpdatedPalettes() || m_update_chosen_pal)
	{
		m_update_chosen_pal = false;

		m_pattern_table[0]->SetData(ppu.GetPatternTable(0, m_bg_pal));
		m_pattern_table[1]->SetData(ppu.GetPatternTable(1, m_sprite_pal));

		std::array<uint32_t, 8 * 4> c;
		for (uint8_t i = 0; i < 8; i++)
		{
			for (uint8_t j = 0; j < 4; j++)
			{
				c[static_cast<size_t>( i * 4 + j )] = ppu.GetColorFromPalette(i, j);
			}
		}

		m_palette[0]->SetData(c.data() + 4 * 0);
		m_palette[1]->SetData(c.data() + 4 * 1);
		m_palette[2]->SetData(c.data() + 4 * 2);
		m_palette[3]->SetData(c.data() + 4 * 3);
		m_palette[4]->SetData(c.data() + 4 * 4);
		m_palette[5]->SetData(c.data() + 4 * 5);
		m_palette[6]->SetData(c.data() + 4 * 6);
		m_palette[7]->SetData(c.data() + 4 * 7);
	}
	if (ppu.HasUpdatedPatternTables())
	{
		m_pattern_table[0]->SetData(ppu.GetPatternTable(0, m_bg_pal));
		m_pattern_table[1]->SetData(ppu.GetPatternTable(1, m_sprite_pal));
	}
}

void Ui::Component::ShowPPUStatus::draw_images()
{
	const float pad = ImGui::GetStyle().FramePadding.x;
	const auto sp = ImGui::GetStyle().ItemSpacing;
	const auto max = ImGui::GetWindowContentRegionMax();
	const auto min = ImGui::GetWindowContentRegionMin();
	const float size_x = ( max.x - min.x - sp.x ) / 2.0f;
	const float pal_x = ( max.x - min.x - sp.x * 7.0f ) / 8.0f;
	const float pal_y = pal_x / 4;

	ImGui::Text("Sprite pattern table");
	ImGui::SameLine(pad + ( size_x + sp.x ) * 1); //ImGui::SetCursorPosX(( size_x + pad ) * 1);
	ImGui::Text("Background pattern table");

	ImGui::Image(Renderer::TextureAsImgui(m_pattern_table[0]), { size_x, size_x }, { 0, 0 }, { 1, 1 }, { 1,1,1,1 }, { 1,1,1,1 });
	ImGui::SameLine(pad + ( size_x + sp.x ) * 1);
	ImGui::Image(Renderer::TextureAsImgui(m_pattern_table[1]), { size_x, size_x }, { 0, 0 }, { 1, 1 }, { 1,1,1,1 }, { 1,1,1,1 });

	ImGui::Separator();
	ImGui::Text("Sprite palettes");
	ImGui::SameLine(pad + ( size_x + sp.x ) * 1); //ImGui::SetCursorPosX(( size_x + sp.x ) * 1);
	ImGui::Text("Background palettes");
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2.0f, 2.0f });
	if (ImGui::ImageButton("pal_4", Renderer::TextureAsImgui(m_palette[4]), { pal_x, pal_y }))
	{
		m_update_chosen_pal = true;
		m_bg_pal = 4;
	}
	ImGui::SameLine(pad + ( pal_x + sp.x ) * 1);
	if (ImGui::ImageButton("pal_5", Renderer::TextureAsImgui(m_palette[5]), { pal_x, pal_y }))
	{
		m_update_chosen_pal = true;
		m_bg_pal = 5;
	}
	ImGui::SameLine(pad + ( pal_x + sp.x ) * 2);
	if (ImGui::ImageButton("pal_6", Renderer::TextureAsImgui(m_palette[6]), { pal_x, pal_y }))
	{
		m_update_chosen_pal = true;
		m_bg_pal = 6;
	}
	ImGui::SameLine(pad + ( pal_x + sp.x ) * 3);
	if (ImGui::ImageButton("pal_7", Renderer::TextureAsImgui(m_palette[7]), { pal_x, pal_y }))
	{
		m_update_chosen_pal = true;
		m_bg_pal = 7;
	}
	ImGui::SameLine(pad + ( pal_x + sp.x ) * 4);
	if (ImGui::ImageButton("pal_0", Renderer::TextureAsImgui(m_palette[0]), { pal_x, pal_y }))
	{
		m_update_chosen_pal = true;
		m_sprite_pal = 0;
	}
	ImGui::SameLine(pad + ( pal_x + sp.x ) * 5);
	if (ImGui::ImageButton("pal_1", Renderer::TextureAsImgui(m_palette[1]), { pal_x, pal_y }))
	{
		m_update_chosen_pal = true;
		m_sprite_pal = 1;
	}
	ImGui::SameLine(pad + ( pal_x + sp.x ) * 6);
	if (ImGui::ImageButton("pal_2", Renderer::TextureAsImgui(m_palette[2]), { pal_x, pal_y }))
	{
		m_update_chosen_pal = true;
		m_sprite_pal = 2;
	}
	ImGui::SameLine(pad + ( pal_x + sp.x ) * 7);
	if (ImGui::ImageButton("pal_3", Renderer::TextureAsImgui(m_palette[3]), { pal_x, pal_y }))
	{
		m_update_chosen_pal = true;
		m_sprite_pal = 3;
	}
	ImGui::PopStyleVar();
}

void Ui::Component::ShowPPUStatus::draw_oam()
{
	if (ImGui::BeginChild("OAM", { 0, 300 }))
	{

		if (ImGui::BeginTable("Str repr", 5, ImGuiTableFlags_BordersH))
		{
			ImGui::TableSetupColumn("Nº");
			ImGui::TableSetupColumn("X pos");
			ImGui::TableSetupColumn("Y pos");
			ImGui::TableSetupColumn("ID");
			ImGui::TableSetupColumn("Attribute");
			ImGui::TableHeadersRow();
			for (size_t i = 0; i < 64; i++)
			{
				const auto& oam = m_oam[i];
				if (!m_is_tracked[i] && oam.y == 0xFF)
				{
					continue;
				}
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("%d", i);
				ImGui::TableNextColumn();
				ImGui::Text("%02X", oam.x);
				ImGui::TableNextColumn();
				ImGui::Text("%02X", oam.y);
				ImGui::TableNextColumn();
				ImGui::Text("%02X", oam.id);
				ImGui::TableNextColumn();
				ImGui::Text("%02X", oam.attribute);
			}
		}
		ImGui::EndTable();

		if (ImGui::BeginPopupContextItem("track"))
		{
			ImGui::Text("Track entry");
			if (m_first_popup_frame)
			{
				m_first_popup_frame = false;
				ImGui::SetKeyboardFocusHere();
			}
			ImGuiInputTextFlags flags =
				ImGuiInputTextFlags_EscapeClearsAll |
				ImGuiInputTextFlags_CharsDecimal |
				ImGuiInputTextFlags_EnterReturnsTrue;
			if (ImGui::InputText("Nº", m_current_track, IM_ARRAYSIZE(m_current_track), flags))
			{
				size_t val = Lud::parse_num<size_t>(m_current_track, 10);
				if (val >= 64)
				{
					Application::Get().Error("Oam out of bounds", std::format("Value {:d} is not in range [0, 63]", val));
				}
				else
				{
					m_is_tracked[val] = true;
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
		else
		{
			std::memset(m_current_track, 0, IM_ARRAYSIZE(m_current_track));
			m_first_popup_frame = true;
		}
	}
	ImGui::EndChild();
}

void Ui::Component::ShowPPUStatus::draw_registers()
{
	if (ImGui::BeginChild("registers", {}, ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_HorizontalScrollbar))
	{
		// tooltips come from https://www.nesdev.org/wiki/PPU_registers#Internal_registers
		ImGui::SeparatorText("Internal registers");

		ImGui::BeginGroup();
		{
			bool latch = m_latch;
			std::string addr = std::format("${:04X}", m_addr);
			std::string temp = std::format("${:04X}", m_temp);
			std::string x = std::format("{:d}", m_finex);

			ImGui::BeginGroup();
			{
				ImGui::Text("V (VRAM address)");
				ImGui::SetItemTooltip("During rendering, used for the scroll position.\nOutside of rendering, used as the current VRAM address.");
				ImGui::BeginDisabled();
				ImGui::InputText("##vram", &addr);
				ImGui::EndDisabled();
				ImGui::SetItemTooltip("15 bits");
				ImGui::EndGroup();
			}

			ImGui::SameLine();
			ImGui::BeginGroup();
			{
				ImGui::Text("T (Temp address)");
				ImGui::SetItemTooltip("During rendering, specifies the starting coarse - x scroll for the next scanline\nand the starting y scroll for the screen.\nOutside of rendering, holds the scroll or VRAM address before transferring it to v.");
				ImGui::BeginDisabled();
				ImGui::InputText("##temp", &temp);
				ImGui::EndDisabled();
				ImGui::SetItemTooltip("15 bits");
				ImGui::EndGroup();
			}
			ImVec2 sz;
			ImGui::BeginGroup();
			{
				ImGui::Text("X (fine x scroll)");
				ImGui::SetItemTooltip("The fine-x position of the current scroll, used during rendering alongside v.");
				sz = ImGui::GetItemRectSize();
				ImGui::BeginDisabled();
				ImGui::InputText("##finex", &x);
				ImGui::EndDisabled();
				ImGui::SetItemTooltip("3 bits");
				ImGui::EndGroup();
			}
			ImGui::SameLine();
			ImGui::BeginGroup();
			{
				ImGui::Dummy(sz);
				ImGui::BeginDisabled();
				ImGui::Checkbox("W (address latch)", &latch);
				ImGui::EndDisabled();
				if (ImGui::BeginItemTooltip())
				{
					ImGui::TextUnformatted("Toggles on each write to either PPUSCROLL or PPUADDR, indicating whether this is the first or second write\nClears on reads of PPUSTATUS\nSometimes called the 'write latch' or 'write toggle'.");
					ImGui::TextUnformatted("\n1 bit");
					ImGui::EndTooltip();
				}
				ImGui::EndGroup();
			}
			ImGui::EndGroup();
		}
		ImGui::SeparatorText("MMIO registers");
		// control register
		ImGui::BeginGroup();
		{
			ImGui::Text(std::format("PPU control: {:08b}", m_control).c_str());
			int nametable = m_control & 0x03;
			bool vram_increment = m_control & 0x4;
			bool sprite_pattern_addr = m_control & 0x08;
			bool sprite_size = m_control & 0x10;
			bool isNmi = m_control & 0x80;
			ImGui::Text("Base nametable address:");
			ImGui::BeginDisabled();
			ImGui::RadioButton("$2000", &nametable, 0);	ImGui::SameLine();
			ImGui::RadioButton("$2400", &nametable, 1); ImGui::SameLine();
			ImGui::RadioButton("$2C00", &nametable, 2);
			ImGui::EndDisabled();

			ImGui::Text("Vram increment:");
			ImGui::BeginDisabled();
			ImGui::RadioButton("1", !vram_increment); ImGui::SameLine();
			ImGui::RadioButton("32", vram_increment);
			ImGui::EndDisabled();

			ImGui::Text("Sprite address 8x8");
			ImGui::SetItemTooltip("Ignored for large sprites");
			ImGui::BeginDisabled();
			ImGui::RadioButton("$0000", !sprite_pattern_addr); ImGui::SameLine();
			ImGui::RadioButton("$1000", sprite_pattern_addr);
			ImGui::EndDisabled();

			ImGui::BeginDisabled();
			ImGui::Checkbox("Large sprites", &sprite_size);
			ImGui::EndDisabled();

			ImGui::SetItemTooltip("Large sprites means sprites are 8x16 pixels");

			ImGui::BeginDisabled();
			ImGui::Checkbox("Generate NMI at vBlank", &isNmi);
			ImGui::EndDisabled();
			ImGui::EndGroup();
		}

		ImGui::SameLine(0, 20);
		// status register
		ImGui::BeginGroup();
		{
			ImGui::Text(std::format("PPU status: {:03b}|{:05b}", m_status >> 5, m_status & 0x1F).c_str());
			ImGui::SetItemTooltip("Low 5 bits are the open bus of the PPU");
			ImGui::BeginDisabled();
			bool isVBlank = m_status & 0x80;
			bool isSprite0Hit = m_status & 0x40;
			bool isSpriteOverflow = m_status & 0x20;
			ImGui::Checkbox("Vblank", &isVBlank);
			ImGui::Checkbox("Sprite 0 hit", &isSprite0Hit);
			ImGui::Checkbox("Sprite Overflow", &isSpriteOverflow);

			ImGui::EndDisabled();
			ImGui::EndGroup();
		}

		ImGui::SameLine(0, 20);

		// mask register
		ImGui::BeginGroup();
		{
			ImGui::Text(std::format("PPU mask: {:08b}", m_mask).c_str());
			ImGui::BeginDisabled();
			bool blue = m_mask & 0x80;
			bool green = m_mask & 0x40;
			bool red = m_mask & 0x20;
			bool showSprites = m_mask & 0x10;
			bool showBackground = m_mask & 0x08;
			bool showSpritesLeft = m_mask & 0x04;
			bool showBackgroundLeft = m_mask & 0x02;
			bool grayscale = m_mask & 0x01;
			ImGui::Checkbox("Emphasize blue", &blue);
			ImGui::Checkbox("Emphasize green", &green);
			ImGui::Checkbox("Emphasize red", &red);
			ImGui::Checkbox("Show sprites", &showSprites);
			ImGui::Checkbox("Show background", &showBackground);
			ImGui::Checkbox("Show sprites left", &showSpritesLeft);
			ImGui::Checkbox("Show background left", &showBackgroundLeft);
			ImGui::Checkbox("Grayscale", &grayscale);

			ImGui::EndDisabled();
			ImGui::EndGroup();
		}
		ImGui::SameLine();
	}
	ImGui::EndChild();
}
