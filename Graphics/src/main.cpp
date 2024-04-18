
#if 0
#include "Application.hpp"
#include <imgui.h>
int main() 
{
	Ui::Application* app = new Ui::Application();
	app->SetMenuCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Load ROM"))
			{
				// TODO:
			}
			if (ImGui::MenuItem("Reset"))
			{
				app->GetConsole().Reset();
			}
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Config"))
		{
			if (ImGui::MenuItem("Load Palette"))
			{
				// TODO:
			}
			ImGui::EndMenu();
			
		}
		if (ImGui::BeginMenu("Debug"))
		{
			if (ImGui::MenuItem("Show Memory"))
			{
				// TODO:
			}
			if (ImGui::MenuItem("Show Debugger"))
			{
				// TODO:
			}
			if (ImGui::MenuItem("Show CPU state"))
			{
				// TODO:
			}
			if (ImGui::MenuItem("Show PPU state"))
			{
				// TODO:
			}
			if (ImGui::MenuItem("Show APU state"))
			{
				// TODO:
			}
			ImGui::EndMenu();
		}
	});
	app->Run();
	delete app;

	return 0;
}
#else
#include <iostream>
#include "Renderer/Engine.hpp"

using Ui::Engine;

int main()
{
	Engine* engine = new Engine();

	engine->Run();

	delete engine;
}

#endif
