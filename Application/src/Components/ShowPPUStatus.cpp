#include "ShowPPUStatus.hpp"
#include <RendererAPI.hpp>

#include "Application.hpp"

#include <imgui.h>
#include <ludutils/lud_parse.hpp>
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
	m_pattern_table[0] = Renderer::CreateTexture(128, 128);
	m_pattern_table[1] = Renderer::CreateTexture(128, 128);

	m_palette[0] = Renderer::CreateTexture(4, 1);
	m_palette[1] = Renderer::CreateTexture(4, 1);
	m_palette[2] = Renderer::CreateTexture(4, 1);
	m_palette[3] = Renderer::CreateTexture(4, 1);

	m_palette[4] = Renderer::CreateTexture(4, 1);
	m_palette[5] = Renderer::CreateTexture(4, 1);
	m_palette[6] = Renderer::CreateTexture(4, 1);
	m_palette[7] = Renderer::CreateTexture(4, 1);
}
void Ui::Component::ShowPPUStatus::OnRender()
{
	if (ImGui::Begin("PPU status", &m_open))
	{
		ImGui::PushFont(m_monospace);
		draw_images();
		const auto sp = ImGui::GetStyle().ItemSpacing;
		const auto avail = ImGui::GetContentRegionAvail();
		const float size_x = ( avail.x - sp.x ) / 2.0f;

		ImVec2 size = { size_x, 0 };
		if (ImGui::BeginChild("cycles and scanlines", size, ImGuiChildFlags_AutoResizeY))
		{
			ImGui::SeparatorText("Cycles");
			ImGui::Text("Scanline: %3d", m_scanlines);
			if (m_scanlines == 261)
			{
				ImGui::SameLine();
				ImGui::Text("Pre render scanline");
			}
			else if (m_scanlines == 240)
			{
				ImGui::SameLine();
				ImGui::Text("Post render scanline");
			}
			else if (m_scanlines > 240)
			{
				ImGui::SameLine();
				ImGui::Text("VBlank scanlines");
			}
			ImGui::Text("Cycle:    %3d", m_cycles);
			if (m_cycles > 255)
			{
				ImGui::SameLine();
				ImGui::Text("HBlank cycles");
			}
			ImGui::Text("Frames:     %d", m_frames);
		}
		ImGui::EndChild();
		ImGui::SameLine();
		if (ImGui::BeginChild("Frame timing", size, ImGuiChildFlags_AutoResizeY))
		{
			ImGui::SeparatorText("Timing");
			ImGui::Text("Frame time:      %5.02fms", m_frame_time);
			if (m_time_since_last == 0)
			{
				ImGui::Text("Time since last: N/A", m_time_since_last);
				ImGui::Text("N/A   FPS");
			}
			else
			{
				ImGui::Text("Time since last: %5.02fms", m_time_since_last);
				ImGui::Text("%02.01f FPS", 1000.0f / m_time_since_last);
			}
		}
		ImGui::EndChild();
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

	auto& ppu = Application::Get().GetConsole().GetPpu();
	auto& bus = Application::Get().GetConsole().GetBus();
	m_control = bus.Peek(0x2000);
	m_mask    = bus.Peek(0x2001);
	m_status  = bus.Peek(0x2002);
	m_cycles  = ppu.GetCycles();
	m_frames  = ppu.GetFrames();
	m_frame_time = static_cast<float>( Application::Get().GetConsole().GetFrameTime() * 1000 );
	m_time_since_last = static_cast<float>( Application::Get().GetConsole().GetTimeSinceLast() * 1000 );
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
				c[static_cast<size_t>(i * 4 + j)] = ppu.GetColorFromPalette(i, j);
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
	const auto avail = ImGui::GetContentRegionAvail();
	const float size_x = ( avail.x - sp.x ) / 2.0f;
	const float pal_x = ( avail.x - sp.x * 7.0f ) / 8.0f;
	const float pal_y = pal_x / 4;

	ImGui::Text("Sprite pattern table");
	ImGui::SameLine(pad + ( size_x + sp.x ) * 1); //ImGui::SetCursorPosX(( size_x + pad ) * 1);
	ImGui::Text("Background pattern table");
	{
		auto pos = ImGui::GetCursorScreenPos();
		ImGui::Image(m_pattern_table[0]->ToImGui(), { size_x, size_x }, { 0, 0 }, { 1, 1 }, { 1,1,1,1 }, { 1,1,1,1 });
		image_tooltip(0, pos, size_x);
	}
	ImGui::SameLine(pad + ( size_x + sp.x ) * 1);
	{
		auto pos = ImGui::GetCursorScreenPos();
		ImGui::Image(m_pattern_table[1]->ToImGui(), { size_x, size_x }, { 0, 0 }, { 1, 1 }, { 1,1,1,1 }, { 1,1,1,1 });
		image_tooltip(1, pos, size_x);
	}

	ImGui::Separator();
	ImGui::Text("Sprite palettes");
	ImGui::SameLine(pad + ( size_x + sp.x ) * 1);
	ImGui::Text("Background palettes");

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2.0f, 2.0f });
	if (ImGui::ImageButton("pal_4", m_palette[4]->ToImGui(), { pal_x, pal_y }))
	{
		m_update_chosen_pal = true;
		m_bg_pal = 4;
	}
	ImGui::SameLine(pad + ( pal_x + sp.x ) * 1);
	if (ImGui::ImageButton("pal_5", m_palette[5]->ToImGui(), { pal_x, pal_y }))
	{
		m_update_chosen_pal = true;
		m_bg_pal = 5;
	}
	ImGui::SameLine(pad + ( pal_x + sp.x ) * 2);
	if (ImGui::ImageButton("pal_6", m_palette[6]->ToImGui(), { pal_x, pal_y }))
	{
		m_update_chosen_pal = true;
		m_bg_pal = 6;
	}
	ImGui::SameLine(pad + ( pal_x + sp.x ) * 3);
	if (ImGui::ImageButton("pal_7", m_palette[7]->ToImGui(), { pal_x, pal_y }))
	{
		m_update_chosen_pal = true;
		m_bg_pal = 7;
	}
	ImGui::SameLine(pad + ( pal_x + sp.x ) * 4);
	if (ImGui::ImageButton("pal_0", m_palette[0]->ToImGui(), { pal_x, pal_y }))
	{
		m_update_chosen_pal = true;
		m_sprite_pal = 0;
	}
	ImGui::SameLine(pad + ( pal_x + sp.x ) * 5);
	if (ImGui::ImageButton("pal_1", m_palette[1]->ToImGui(), { pal_x, pal_y }))
	{
		m_update_chosen_pal = true;
		m_sprite_pal = 1;
	}
	ImGui::SameLine(pad + ( pal_x + sp.x ) * 6);
	if (ImGui::ImageButton("pal_2", m_palette[2]->ToImGui(), { pal_x, pal_y }))
	{
		m_update_chosen_pal = true;
		m_sprite_pal = 2;
	}
	ImGui::SameLine(pad + ( pal_x + sp.x ) * 7);
	if (ImGui::ImageButton("pal_3", m_palette[3]->ToImGui(), { pal_x, pal_y }))
	{
		m_update_chosen_pal = true;
		m_sprite_pal = 3;
	}
	ImGui::PopStyleVar();
}

void Ui::Component::ShowPPUStatus::draw_oam()
{
	if (ImGui::BeginChild("OAM", { 0, 300 }, 0, ImGuiWindowFlags_HorizontalScrollbar))
	{
		constexpr ImGuiTableFlags t_flags =
			ImGuiTableFlags_Borders |
			ImGuiTableFlags_SizingFixedFit |
			ImGuiTableFlags_RowBg |
			ImGuiTableFlags_Resizable |
			ImGuiTableFlags_Hideable |
			ImGuiTableFlags_Reorderable;
		if (ImGui::BeginTable("Str repr", 5, t_flags))
		{
			ImGui::TableSetupColumn("Nº ", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("X  ", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Y  ", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("ID ", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Attribute", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();
			for (size_t i = 0; i < 64; i++)
			{
				const auto& oam = m_oam[i];
				if (!m_is_tracked[i] && oam.y > static_cast<u8>( Application::Get().GetConsole().GetConfig().height ))
				{
					continue;
				}
				// sprite debugging
				//{
				//	const ImVec2 px_dim = { 256, 240 };
				//	auto dim = Application::Get().GetScreenSize() / px_dim;
				//	const auto work_pos = ImGui::GetMainViewport()->WorkPos;
				//	const auto work_size = ImGui::GetMainViewport()->WorkSize;
				//	const ImVec2 oam_pos = { dim.x * oam.x, dim.y * oam.y };
				//	ImGui::GetForegroundDrawList()->AddRect(
				//		work_pos + oam_pos,
				//		work_pos + oam_pos + ImVec2(8, 8) * dim,
				//		IM_COL32_WHITE,
				//		0,
				//		0,
				//		3.0f
				//	);
				//	auto text_size = ImGui::CalcTextSize("01");
				//	auto text_pos = work_pos + oam_pos + ( ImVec2(8, 8) * dim - text_size ) / 2;
				//	ImGui::GetForegroundDrawList()->AddText(
				//		text_pos,
				//		IM_COL32_WHITE,
				//		std::format("{:>2d}", i).c_str()
				//	);
				//}
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("%d", i);
				ImGui::TableNextColumn();
				ImGui::Text("%03d", oam.x);
				ImGui::TableNextColumn();
				ImGui::Text("%03d", oam.y);
				ImGui::TableNextColumn();
				ImGui::Text("%02X", oam.id);
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(std::format("{:08b}", oam.attribute).c_str());
				int palette = oam.attribute & 0x03;
				bool behind = oam.attribute & 0x20;
				bool flip_h = oam.attribute & 0x40;
				bool flip_v = oam.attribute & 0x80;

				ImGui::Text("Palette");
				auto sz_y = ImGui::GetItemRectSize().y;
				ImGui::SameLine();
				ImGui::Image(m_palette[4 + palette]->ToImGui(), { sz_y * 4, sz_y }, { 0, 0 }, { 1, 1 }, { 1,1,1,1 }, { 1,1,1,1 });
				ImGui::BeginDisabled();
				{
					ImGui::Checkbox("Behind background", &behind);
					ImGui::Checkbox("Flip vertically", &flip_v);
					ImGui::SameLine();
					ImGui::Checkbox("Flip horizontally", &flip_h);
				}
				ImGui::EndDisabled();
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
			std::fill_n(m_current_track, IM_ARRAYSIZE(m_current_track), 0);
			m_first_popup_frame = true;
		}
	}
	ImGui::EndChild();
}

void Ui::Component::ShowPPUStatus::draw_registers()
{

	const auto sp = ImGui::GetStyle().ItemSpacing;
	if (ImGui::BeginChild("registers", {}, ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_HorizontalScrollbar))
	{
		// tooltips come from https://www.nesdev.org/wiki/PPU_registers#Internal_registers
		ImGui::SeparatorText("Internal registers");

		bool latch = m_latch;
		std::string addr = std::format("${:04X}", m_addr);
		std::string temp = std::format("${:04X}", m_temp);
		std::string x = std::format("{:d}", m_finex);
		const auto avail = ImGui::GetContentRegionAvail();
		const float size_x = ( avail.x - sp.x ) / 2.0f;

		if (ImGui::BeginChild("VT", { size_x, 0 }, ImGuiChildFlags_AutoResizeY))
		{
			ImGui::Text("V (VRAM address)");
			ImGui::SetItemTooltip("During rendering, used for the scroll position.\nOutside of rendering, used as the current VRAM address.");

			ImGui::BeginDisabled();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::InputText("##vram", &addr);
			ImGui::EndDisabled();

			ImGui::SetItemTooltip("15 bits");
			ImGui::Text("T (Temp address)");
			ImGui::SetItemTooltip("During rendering, specifies the starting coarse - x scroll for the next scanline\nand the starting y scroll for the screen.\nOutside of rendering, holds the scroll or VRAM address before transferring it to v.");

			ImGui::BeginDisabled();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::InputText("##temp", &temp);
			ImGui::EndDisabled();

			ImGui::SetItemTooltip("15 bits");

		}
		ImGui::EndChild();

		ImGui::SameLine();
		if (ImGui::BeginChild("XW", { size_x, 0 }, ImGuiChildFlags_AutoResizeY))
		{
			ImGui::Text("X (fine x scroll)");
			ImVec2 sz = ImGui::GetItemRectSize();
			ImGui::SetItemTooltip("The fine-x position of the current scroll, used during rendering alongside v.");

			ImGui::BeginDisabled();
			ImGui::SetNextItemWidth(-FLT_MIN);
			ImGui::InputText("##finex", &x);
			ImGui::EndDisabled();

			ImGui::SetItemTooltip("3 bits");
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
		}
		ImGui::EndChild();
	}
	ImGui::EndChild();
	ImGuiChildFlags c_flags = ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY;
	if (ImGui::BeginChild("MMIO", { }, c_flags))
	{
		const auto avail = ImGui::GetContentRegionAvail();
		const float size_x = ( avail.x - sp.x * 2.0f ) / 3.0f;

		ImGui::SeparatorText("MMIO registers");
		// control register
		if (ImGui::BeginChild("ctrl", { size_x, 0 }, c_flags, ImGuiWindowFlags_HorizontalScrollbar))
		{
			int nametable             = m_control & Emu::Control::NAMETABLE_BASE_ADDR;
			bool vram_increment       = m_control & Emu::Control::VRAM_INCREMENT;
			bool sprite_pattern_addr  = m_control & Emu::Control::SPRITE_PATTERN_ADDR;
			bool sprite_size          = m_control & Emu::Control::LARGE_SPRITES;
			bool isNmi                = m_control & Emu::Control::GENERATE_NMI;
			int bg_pattern_table_addr = m_control & Emu::Control::BACKGROUND_PATTERN_ADDR;
			ImGui::Text(std::format("PPU control: {:08b}", m_control).c_str());
			ImGui::Text("Base nametable address:");
			ImGui::BeginDisabled();
			{
				ImGui::RadioButton("$2000", &nametable, 0);	//ImGui::SameLine();
				ImGui::RadioButton("$2400", &nametable, 1); //ImGui::SameLine();
				ImGui::RadioButton("$2C00", &nametable, 2);
			}
			ImGui::EndDisabled();

			ImGui::Text("Vram increment:");
			ImGui::BeginDisabled();
			{
				ImGui::RadioButton("1", !vram_increment); ImGui::SameLine();
				ImGui::RadioButton("32", vram_increment);
			}
			ImGui::EndDisabled();

			ImGui::Text("Sprite address 8x8");
			ImGui::SetItemTooltip("Ignored for large sprites");
			ImGui::BeginDisabled();
			{
				ImGui::RadioButton("$0000", !sprite_pattern_addr); ImGui::SameLine();
				ImGui::RadioButton("$1000", sprite_pattern_addr);
			}
			ImGui::EndDisabled();

			ImGui::Text("Background address");
			ImGui::BeginDisabled();
			{
				ImGui::RadioButton("$0000", !bg_pattern_table_addr); ImGui::SameLine();
				ImGui::RadioButton("$1000", bg_pattern_table_addr);
			}
			ImGui::EndDisabled();

			ImGui::BeginDisabled();
			{
				ImGui::Checkbox("Large sprites", &sprite_size);
			}
			ImGui::EndDisabled();

			ImGui::SetItemTooltip("Large sprites means sprites are 8x16 pixels");

			ImGui::BeginDisabled();
			{
				ImGui::Checkbox("NMI at vBlank", &isNmi);
			}
			ImGui::EndDisabled();
			ImGui::Dummy(ImGui::CalcTextSize("A"));


		}
		ImGui::EndChild();

		ImGui::SameLine();
		// status register
		if (ImGui::BeginChild("status", { size_x, 0 }, c_flags, ImGuiWindowFlags_HorizontalScrollbar))
		{
			bool isVBlank         = m_status & Emu::Status::VERTICAL_BLANK;
			bool isSprite0Hit     = m_status & Emu::Status::SPRITE_0_HIT;
			bool isSpriteOverflow = m_status & Emu::Status::SPRITE_OVERFLOW;
			ImGui::Text(std::format("PPU status: {:03b}|{:05b}", m_status >> 5, m_status & 0x1F).c_str());
			ImGui::SetItemTooltip("Low 5 bits are the open bus of the PPU");
			ImGui::BeginDisabled();
			{
				ImGui::Checkbox("Vblank", &isVBlank);
				ImGui::Checkbox("Sprite 0 hit", &isSprite0Hit);
				ImGui::Checkbox("Sprite Overflow", &isSpriteOverflow);
			}
			ImGui::EndDisabled();
		}
		ImGui::EndChild();

		ImGui::SameLine();

		// mask register
		if (ImGui::BeginChild("mask", { size_x, 0 }, c_flags, ImGuiWindowFlags_HorizontalScrollbar))
		{
			bool blue               = m_mask & Emu::Mask::EMPHASIZE_BLUE;
			bool green              = m_mask & Emu::Mask::EMPHASIZE_GREEN;
			bool red                = m_mask & Emu::Mask::EMPHASIZE_RED;
			bool showSprites        = m_mask & Emu::Mask::SHOW_SPRITES;
			bool showBackground     = m_mask & Emu::Mask::SHOW_BACKGROUND;
			bool showSpritesLeft    = m_mask & Emu::Mask::SHOW_LEFT_SPRITE;
			bool showBackgroundLeft = m_mask & Emu::Mask::SHOW_LEFT_BACKGROUND;
			bool grayscale          = m_mask & Emu::Mask::GRAYSCALE;
			ImGui::Text(std::format("PPU mask: {:08b}", m_mask).c_str());
			ImGui::BeginDisabled();
			{
				ImGui::Checkbox("Emphasize blue", &blue);
				ImGui::Checkbox("Emphasize green", &green);
				ImGui::Checkbox("Emphasize red", &red);
				ImGui::Checkbox("Show sprites", &showSprites);
				ImGui::Checkbox("Show background", &showBackground);
				ImGui::Checkbox("Show sprites left", &showSpritesLeft);
				ImGui::Checkbox("Show background left", &showBackgroundLeft);
				ImGui::Checkbox("Grayscale", &grayscale);
			}
			ImGui::EndDisabled();
			ImGui::Dummy(ImGui::CalcTextSize("A"));
		}
		ImGui::EndChild();
	}
	ImGui::EndChild();
}

void Ui::Component::ShowPPUStatus::image_tooltip(const uint32_t pat, const ImVec2 pos, const float size) const
{
	auto& io = ImGui::GetIO();
	if (!io.MouseDown[0])
	{
		return;
	}

	if (ImGui::BeginItemTooltip())
	{
		const auto mpos = io.MousePos;
		const float zoom = 8.0f;
		const ImVec2 v1 = { 1.0f, 1.0f };
		const ImVec2 dim = { 16.0f * zoom, 16.0f * zoom };
		const ImVec2 vsize = { size, size };
		const ImVec2 sector = {
			std::floorf(( mpos.x - pos.x ) / size * 16.0f),
			std::floorf(( mpos.y - pos.y ) / size * 16.0f),
		};
		const ImVec2 uv0 = sector / 16.0f;
		const ImVec2 uv1 = ( sector + v1 ) / 16.0f;

		const auto w_size = ImGui::GetWindowSize();
		const int pat_pos = static_cast<int>( sector.x + sector.y * 16 );
		const u16 pat_addr = pat_pos * 0x10 + 0x1000 * pat;

		ImGui::Text("pattern: $%04X", pat_addr);
		ImGui::SetCursorPosX(( w_size.x - dim.x ) / 2);

		float n_pos = ImGui::GetCursorPosX();
		float min_x = ImGui::GetWindowContentRegionMin().x;

		ImGui::Image(m_pattern_table[pat]->ToImGui(), dim, uv0, uv1);
		//adding some padding same size of border under image
		ImGui::Dummy({ 0, ( n_pos - min_x ) / 2 });

		ImGui::EndTooltip();
	}
}
