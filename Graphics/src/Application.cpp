#include "Application.hpp"

#include <functional>
#include <iostream>
#include <print>
#include <vector>

#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_vulkan.h>
#include <SDL.h>

#include "Components/IComponent.hpp"
#include "Renderer/RendererAPI.hpp"
#include "Window/SDL/SDLWindow.hpp"	

#include "Renderer/Vulkan/VulkanTexture.hpp"
#include <pfd/portable_file_dialogs.h>


namespace Ui
{
Application* s_instance;

Application::Application(const Configuration& config)
	: m_config(config)
	, m_console(Emu::NTSC)
{
	m_window = new SDLWindow(m_config.name, m_config.w, m_config.h);
	s_instance = this;
	init();
	m_texel_data = Renderer::CreateTexture(
		static_cast<uint32_t>( m_console.GetConfig().width ),
		static_cast<uint32_t>( m_console.GetConfig().height )
	);
	m_emu_screen.SetTexture(m_texel_data);
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
	delete m_texel_data;
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
	while (!m_should_quit)
	{
		event_loop();

		if (m_stop_rendering)
		{
			continue;
		}
		Renderer::Resize();

		update();

		draw_ui();
		draw_application();
	}
}
void Application::event_loop()
{
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		if (event.type == SDL_QUIT)
			m_should_quit = true;

		if (event.type == SDL_WINDOWEVENT)
		{
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				Renderer::RequestResize();
				m_resize_emu_screen = true;
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
}
void Application::draw_ui()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame();

	ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Load ROM"))
			{
				// TODO:
				auto f = pfd::open_file("Chose ROM File", pfd::path::home(),
					{
						"Rom Files (.nes, .ines)", "*.nes",
							"All files", "*"
					});

				if (!f.result().empty())
				{
					m_console.LoadCartridge(f.result()[0]);
				}
			}
			if (ImGui::MenuItem("Reset"))
			{
				m_console.Reset();
			}
			if (ImGui::MenuItem("Exit"))
			{
				Close();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	ImGui::Render();
	// Update and Render additional Platform Windows
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}
void Application::draw_application()
{
	m_emu_screen.Draw();

	Renderer::Draw();
}
void Application::update()
{
	if (m_console.RunFrame())
	{
	}
	m_texel_data->SetData(m_console.OutputScreen());
	if (m_resize_emu_screen)
	{
		m_resize_emu_screen = false;
		resize_emu_screen();
	}
}
void Application::resize_emu_screen()
{
	constexpr float menu_bar_height = 26;

	const auto [window_width, window_height] = m_window->GetDimensions();
	const float w = static_cast<float>( window_width );
	// taking into account menu bar
	const float h = static_cast<float>( window_height - menu_bar_height );

	const float nes_aspect_ratio = m_console.GetConfig().width / m_console.GetConfig().height;

	Rect new_bounds;
	// NES resolution is not 1 so i have to check window resolution against nes resolution in order to 
	// check if width or height should be used for calculations
	if (w / h > nes_aspect_ratio)
	{
		new_bounds.w = h * nes_aspect_ratio;
		new_bounds.h = h;
		new_bounds.x = w / 2.0f - new_bounds.w / 2.0f;
		new_bounds.y = menu_bar_height;
	}
	else
	{
		new_bounds.w = w;
		new_bounds.h = w / nes_aspect_ratio;
		new_bounds.x = 0;
		new_bounds.y = h / 2.0f - new_bounds.h / 2.0f + menu_bar_height;
	}

	m_emu_screen.rect = new_bounds;
}
}

