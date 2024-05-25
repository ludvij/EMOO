#include "Renderer/RendererAPI.hpp"
#include "ShowPPUStatus.hpp"

#include "Application.hpp"

Ui::Component::ShowPPUStatus::ShowPPUStatus(const std::string_view name)
	: IComponent(name)
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
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 10.0f, ImGui::GetStyle().FramePadding.y });
	if (ImGui::Begin("PPU status", &m_open))
	{
		const float pad = ImGui::GetStyle().FramePadding.x;
		const float size_x = ( ImGui::GetWindowSize().x - pad * 3 ) / 2.0f;
		const float pal_x = ( ImGui::GetWindowSize().x - pad * 9.0f ) / 8.0f;
		const float pal_y = pal_x / 4;

		ImGui::Text("Sprite pattern table");
		ImGui::SameLine(); ImGui::SetCursorPosX(pad + ( size_x + pad ) * 1);
		ImGui::Text("Background pattern table");

		ImGui::SetCursorPosX(pad);
		ImGui::Image(Renderer::TextureAsImgui(m_pattern_table[0]), { size_x, size_x }, { 0, 0 }, { 1, 1 }, { 1,1,1,1 }, { 1,1,1,1 });
		ImGui::SameLine(); ImGui::SetCursorPosX(pad + ( size_x + pad ) * 1);
		ImGui::Image(Renderer::TextureAsImgui(m_pattern_table[1]), { size_x, size_x }, { 0, 0 }, { 1, 1 }, { 1,1,1,1 }, { 1,1,1,1 });

		ImGui::Text("Sprite palettes");
		ImGui::SameLine(); ImGui::SetCursorPosX(pad + ( size_x + pad ) * 1);
		ImGui::Text("Background palettes");

		ImGui::SetCursorPosX(pad);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2.0f, 2.0f });
		if (ImGui::ImageButton("pal_4", Renderer::TextureAsImgui(m_palette[4]), { pal_x, pal_y }))
		{
			m_update_chosen_pal = true;
			m_bg_pal = 4;
		}
		ImGui::SameLine(); ImGui::SetCursorPosX(pad + ( pal_x + pad ) * 1);
		if (ImGui::ImageButton("pal_5", Renderer::TextureAsImgui(m_palette[5]), { pal_x, pal_y }))
		{
			m_update_chosen_pal = true;
			m_bg_pal = 5;
		}
		ImGui::SameLine(); ImGui::SetCursorPosX(pad + ( pal_x + pad ) * 2);
		if (ImGui::ImageButton("pal_6", Renderer::TextureAsImgui(m_palette[6]), { pal_x, pal_y }))
		{
			m_update_chosen_pal = true;
			m_bg_pal = 6;
		}
		ImGui::SameLine(); ImGui::SetCursorPosX(pad + ( pal_x + pad ) * 3);
		if (ImGui::ImageButton("pal_7", Renderer::TextureAsImgui(m_palette[7]), { pal_x, pal_y }))
		{
			m_update_chosen_pal = true;
			m_bg_pal = 7;
		}
		ImGui::SameLine(); ImGui::SetCursorPosX(pad + ( pal_x + pad ) * 4);
		if (ImGui::ImageButton("pal_0", Renderer::TextureAsImgui(m_palette[0]), { pal_x, pal_y }))
		{
			m_update_chosen_pal = true;
			m_sprite_pal = 0;
		}
		ImGui::SameLine(); ImGui::SetCursorPosX(pad + ( pal_x + pad ) * 5);
		if (ImGui::ImageButton("pal_1", Renderer::TextureAsImgui(m_palette[1]), { pal_x, pal_y }))
		{
			m_update_chosen_pal = true;
			m_sprite_pal = 1;
		}
		ImGui::SameLine(); ImGui::SetCursorPosX(pad + ( pal_x + pad ) * 6);
		if (ImGui::ImageButton("pal_2", Renderer::TextureAsImgui(m_palette[2]), { pal_x, pal_y }))
		{
			m_update_chosen_pal = true;
			m_sprite_pal = 2;
		}
		ImGui::SameLine(); ImGui::SetCursorPosX(pad + ( pal_x + pad ) * 7);
		if (ImGui::ImageButton("pal_3", Renderer::TextureAsImgui(m_palette[3]), { pal_x, pal_y }))
		{
			m_update_chosen_pal = true;
			m_sprite_pal = 3;
		}
		ImGui::PopStyleVar();
	}
	ImGui::End();
	ImGui::PopStyleVar();
}


void Ui::Component::ShowPPUStatus::OnUpdate()
{
	if (!m_open)
	{
		removed = true;
		return;
	}
	auto& ppu = Application::GetConsole().GetPpu();
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
