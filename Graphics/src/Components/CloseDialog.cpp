#include "CloseDialog.hpp"
#include <imgui.h>

#include "Application.hpp"

namespace Ui::Component
{


CloseDialog::CloseDialog(const char* msg, bool close)
	: m_msg(msg)
	, m_close(close)
{
}

CloseDialog::~CloseDialog()
{
	Application::SetUpdate(true);
}

void CloseDialog::OnCreate()
{
	Application::Get().RestartEmulator();
	Application::SetUpdate(false);
}

void CloseDialog::OnRender()
{
	ImGui::SetNextWindowSize({ 400, 250 }, ImGuiCond_Once);
	if (ImGui::Begin("An error has occured", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
	{
		ImGui::Text(m_msg);
		if (m_close)
		{
			ImGui::Text("Closing application");
		}

		auto label = "Accept";
		ImGuiStyle& style = ImGui::GetStyle();
		auto t_size = ImGui::CalcTextSize(label);
		ImVec2 avail = ImGui::GetContentRegionAvail();

		float size_x = t_size.x + style.FramePadding.x * 2.0f;
		float size_y = t_size.y + style.FramePadding.y * 2.0f;

		float off_x = ( avail.x - size_x );
		float off_y = ( avail.y - size_y );

		if (off_x > 0.0f)
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off_x);
		}
		if (off_y > 0.0f)
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + off_y);

		}

		if (ImGui::Button(label))
		{
			if (m_close)
			{
				Application::Get().Close();
			}
			m_removed = true;
		}
	}
	ImGui::End();
}

}
