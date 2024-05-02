#include "Application.hpp"

#include <functional>
#include <iostream>
#include <vector>

#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_vulkan.h>
#include <SDL.h>

#include "Components/IComponent.hpp"
#include "Renderer/RendererAPI.hpp"
#include "Window/SDL/SDLWindow.hpp"	


namespace Ui
{
Application* s_instance;

Application::Application(const Configuration& config)
	: m_config(config)
	, m_console(Emu::NTSC)
{
	m_window = new SDLWindow(m_config.w, m_config.h);
	s_instance = this;
	init();
}

Application::~Application()
{
	shutdown();

	s_instance = nullptr;
}

Application& Application::Get()
{
	return *s_instance;
}

void Application::init()
{
	Renderer::Init(m_window, true);

}

void Application::shutdown()
{
	delete m_window;
}


void Application::Run()
{
	main_loop();
}

void Application::Close()
{
	m_should_quit = true;
}

void Application::main_loop()
{
	SDL_Event event;
	while (!m_should_quit)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
				m_should_quit = true;

			if (event.type == SDL_WINDOWEVENT)
			{
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					Renderer::RequestResize();
				}
				if (event.window.event == SDL_WINDOWEVENT_MINIMIZED)
				{
					m_stop_rendering = true;
				}
				if (event.window.event == SDL_WINDOWEVENT_RESTORED)
				{
					m_stop_rendering = false;
				}
				if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == m_window->GetWindowID())
				{
					m_should_quit = true;
				}
			}
			ImGui_ImplSDL2_ProcessEvent(&event);
		}


		if (m_stop_rendering)
		{
			// slow loop
			continue;
		}
		Renderer::Resize();

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame();

		ImGui::NewFrame();

		if (ImGui::Begin("Position"))
		{
			auto [w, h] = m_window->GetDimensions();

			if (ImGui::BeginTable("Window size", 2))
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text("w: %d", w);
				ImGui::TableNextColumn();
				ImGui::Text("h: %d", h);

				ImGui::EndTable();
			}
		}
		ImGui::End();

		ImGui::Render();

		Sprite test({ 10.0f, 10.0f, 200.0f, 200.0f }, 0.0f);

		test.Draw();

		Renderer::Draw();

		// Update and Render additional Platform Windows
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

	}
}
}

