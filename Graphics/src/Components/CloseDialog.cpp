#include "CloseDialog.hpp"
#include <imgui.h>

#include <string_view>

#include "Application.hpp"

namespace Ui::Component
{


CloseDialog::CloseDialog(const std::string_view name, const char* msg, bool close)
	: IComponent(name)
	, m_msg(msg)
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
	if (m_frame++ == 0)
	{
		ImGui::OpenPopup("Error");
	}
	ImVec2 pad = ImGui::GetStyle().FramePadding;
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("An error has occurred");
		ImGui::Text(m_msg);
		if (m_close)
		{
			ImGui::Text("Closing application");
		}
		ImGui::Separator();
		auto elem = ImGui::GetWindowSize();
		if (ImGui::Button("Accept", { ( elem.x - pad.x * 2 ) / 2, 0 }))
		{
			if (m_close)
			{
				Application::Get().Close();
			}
			removed = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

}
