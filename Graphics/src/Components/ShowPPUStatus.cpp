#include "Renderer/RendererAPI.hpp"
#include "ShowPPUStatus.hpp"

#include "Application.hpp"

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
	//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 10.0f, ImGui::GetStyle().FramePadding.y });
	if (ImGui::Begin("PPU status", &m_open))
	{
		ImGui::PushFont(m_monospace);
		draw_images();
		ImGui::Separator();
		ImGui::Text("PPU cycle:    %d", m_cycles);
		ImGui::Text("PPU scanline: %d", m_scanlines);
		ImGui::Separator();

		if (ImGui::BeginChild("registers", {}, ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar))
		{
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
	m_scanlines = ppu.GetScanlines();
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
