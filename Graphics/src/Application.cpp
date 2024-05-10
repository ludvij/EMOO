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

#include <chrono>
#include <thread>


namespace Ui
{
Application* s_instance;

Application::Application(const Configuration& config)
	: m_config(config)
	, m_console(Emu::NTSC)
{
	s_instance = this;
	m_window = new SDLWindow(m_config.name, m_config.w, m_config.h);
	init();

	const auto w = static_cast<uint32_t>( m_console.GetConfig().width );
	const auto h = static_cast<uint32_t>( m_console.GetConfig().height );

	m_textures["SCREEN"]          = std::unique_ptr<ITexture>(Renderer::CreateTexture(w, h));
	m_textures["PATTERN_TABLE_0"] = std::unique_ptr<ITexture>(Renderer::CreateTexture(128, 128));
	m_textures["PATTERN_TABLE_1"] = std::unique_ptr<ITexture>(Renderer::CreateTexture(128, 128));
	m_textures["PALETTE"]         = std::unique_ptr<ITexture>(Renderer::CreateTexture(16, 4));
	m_textures["USED_PALETTE"]    = std::unique_ptr<ITexture>(Renderer::CreateTexture(4 * 8, 1));

	m_sprites["SCREEN"]          = Sprite({}, 0, m_textures["SCREEN"].get());
	m_sprites["PATTERN_TABLE_0"] = Sprite({}, 0, m_textures["PATTERN_TABLE_0"].get());
	m_sprites["PATTERN_TABLE_1"] = Sprite({}, 0, m_textures["PATTERN_TABLE_1"].get());
	m_sprites["PALETTE"]         = Sprite({}, 0, m_textures["PALETTE"].get());
	m_sprites["USED_PALETTE_0"]  = Sprite({}, 0, m_textures["USED_PALETTE"].get(), { .125 * 0, 0, .125 * 1, 1 });
	m_sprites["USED_PALETTE_1"]  = Sprite({}, 0, m_textures["USED_PALETTE"].get(), { .125 * 1, 0, .125 * 2, 1 });
	m_sprites["USED_PALETTE_2"]  = Sprite({}, 0, m_textures["USED_PALETTE"].get(), { .125 * 2, 0, .125 * 3, 1 });
	m_sprites["USED_PALETTE_3"]  = Sprite({}, 0, m_textures["USED_PALETTE"].get(), { .125 * 3, 0, .125 * 4, 1 });
	m_sprites["USED_PALETTE_4"]  = Sprite({}, 0, m_textures["USED_PALETTE"].get(), { .125 * 4, 0, .125 * 5, 1 });
	m_sprites["USED_PALETTE_5"]  = Sprite({}, 0, m_textures["USED_PALETTE"].get(), { .125 * 5, 0, .125 * 6, 1 });
	m_sprites["USED_PALETTE_6"]  = Sprite({}, 0, m_textures["USED_PALETTE"].get(), { .125 * 6, 0, .125 * 7, 1 });
	m_sprites["USED_PALETTE_7"]  = Sprite({}, 0, m_textures["USED_PALETTE"].get(), { .125 * 7, 0, .125 * 8, 1 });
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
	while (!m_should_quit)
	{
		const auto start = std::chrono::time_point<std::chrono::steady_clock>(std::chrono::steady_clock::now());

		event_loop();

		if (m_stop_rendering)
		{
			continue;
		}
		Renderer::Resize();

		update();

		draw_ui();
		draw_application();

		auto end = std::chrono::time_point<std::chrono::steady_clock>(std::chrono::steady_clock::now());
		auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count();

		const auto ms_per_frame = static_cast<int>( 1000.0f / m_console.GetConfig().FrameRate );
		if (ms_per_frame > elapsed_time)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(ms_per_frame - elapsed_time));
		}

		end = std::chrono::time_point<std::chrono::steady_clock>(std::chrono::steady_clock::now());
		elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count();
		m_frame_rate = static_cast<uint32_t>( 1000.0f / elapsed_time );
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
				m_resized = true;
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
		m_menu_bar_height= ImGui::GetWindowSize().y;
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
		ImGui::Separator();
		ImGui::Text("FPS %2d", m_frame_rate);
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
	for (auto& [k, v] : m_sprites)
	{
		v.Draw();
	}

	Renderer::Draw();
}
void Application::update()
{
	if (m_console.RunFrame())
	{
	}
	get_pixel_data();

	if (m_resized)
	{
		m_resized = false;
		resize_emu_screen();
		resize_other();
	}
}
void Application::get_pixel_data()
{
	m_textures["SCREEN"]->SetData(m_console.OutputScreen());

	m_textures["PALETTE"]->SetData(m_console.OutputPalette());
	m_textures["PATTERN_TABLE_0"]->SetData(m_console.OutputPatternTable(0, 0));
	m_textures["PATTERN_TABLE_1"]->SetData(m_console.OutputPatternTable(1, 0));

	std::array<u32, 4 * 8> pal;
	for (uint8_t p = 0; p < 8; p++)
	{
		for (uint8_t s = 0; s < 4; s++)
		{
			pal[static_cast<size_t>( s + p * 4 )] = m_console.OutputPaletteColor(p, s);
		}
	}
	m_textures["USED_PALETTE"]->SetData(pal.data());
}
void Application::resize_emu_screen()
{

	const auto [window_width, window_height] = m_window->GetDimensions();
	const float w = static_cast<float>( window_width );
	// taking into account menu bar
	const float h = static_cast<float>( window_height - m_menu_bar_height );

	const float nes_aspect_ratio = m_console.GetConfig().width / m_console.GetConfig().height;

	Rect new_bounds;
	// NES resolution is not 1 so i have to check window resolution against nes resolution in order to 
	// check if width or height should be used for calculations
	if (w / h > nes_aspect_ratio)
	{
		new_bounds.w = h * nes_aspect_ratio;
		new_bounds.h = h;
	}
	else
	{
		new_bounds.w = w;
		new_bounds.h = w / nes_aspect_ratio;
	}
	new_bounds.x = 0;
	new_bounds.y = m_menu_bar_height;

	m_sprites["SCREEN"].rect = new_bounds;
}
void Application::resize_other()
{
	const auto [window_width, window_height] = m_window->GetDimensions();
	const float w = static_cast<float>( window_width );
	// taking into account menu bar
	const float h = static_cast<float>( window_height - m_menu_bar_height );
	const auto nes_bounds = m_sprites["SCREEN"].rect;

	constexpr float padding = 10;

	if (w / h > nes_bounds.w / nes_bounds.h)
	{
		const float pattern_w = ( w - nes_bounds.w - padding * 3.0f ) / 2.0f;

		const float palette_w = pattern_w * 2 + padding;
		const float palette_h = palette_w / 16 * 4;

		Rect new_bounds = { nes_bounds.w + padding, m_menu_bar_height + padding, pattern_w, pattern_w };
		m_sprites["PATTERN_TABLE_0"].rect = { new_bounds.x + ( new_bounds.w + padding ) * 0, new_bounds.y, new_bounds.w, new_bounds.h };
		m_sprites["PATTERN_TABLE_1"].rect = { new_bounds.x + ( new_bounds.w + padding ) * 1, new_bounds.y, new_bounds.w, new_bounds.h };
		m_sprites["PALETTE"].rect = { nes_bounds.w + padding, m_menu_bar_height + h - palette_h - padding, palette_w, palette_h };

		const float pal_w = ( ( pattern_w * 2 + padding ) - padding * 7 ) / 8.0f;
		const float pal_h = pal_w / 4.0f;
		m_sprites["USED_PALETTE_0"].rect = { nes_bounds.w + padding + ( pal_w + padding ) * 0, new_bounds.y + new_bounds.h + padding, pal_w, pal_h };
		m_sprites["USED_PALETTE_1"].rect = { nes_bounds.w + padding + ( pal_w + padding ) * 1, new_bounds.y + new_bounds.h + padding, pal_w, pal_h };
		m_sprites["USED_PALETTE_2"].rect = { nes_bounds.w + padding + ( pal_w + padding ) * 2, new_bounds.y + new_bounds.h + padding, pal_w, pal_h };
		m_sprites["USED_PALETTE_3"].rect = { nes_bounds.w + padding + ( pal_w + padding ) * 3, new_bounds.y + new_bounds.h + padding, pal_w, pal_h };
		m_sprites["USED_PALETTE_4"].rect = { nes_bounds.w + padding + ( pal_w + padding ) * 4, new_bounds.y + new_bounds.h + padding, pal_w, pal_h };
		m_sprites["USED_PALETTE_5"].rect = { nes_bounds.w + padding + ( pal_w + padding ) * 5, new_bounds.y + new_bounds.h + padding, pal_w, pal_h };
		m_sprites["USED_PALETTE_6"].rect = { nes_bounds.w + padding + ( pal_w + padding ) * 6, new_bounds.y + new_bounds.h + padding, pal_w, pal_h };
		m_sprites["USED_PALETTE_7"].rect = { nes_bounds.w + padding + ( pal_w + padding ) * 7, new_bounds.y + new_bounds.h + padding, pal_w, pal_h };
	}
	else
	{
		m_sprites["PATTERN_TABLE_0"].rect = {};
		m_sprites["PATTERN_TABLE_1"].rect = {};
		m_sprites["PALETTE"].rect = {};
		m_sprites["USED_PALETTE_0"].rect = {};
		m_sprites["USED_PALETTE_1"].rect = {};
		m_sprites["USED_PALETTE_2"].rect = {};
		m_sprites["USED_PALETTE_3"].rect = {};
		m_sprites["USED_PALETTE_4"].rect = {};
		m_sprites["USED_PALETTE_5"].rect = {};
		m_sprites["USED_PALETTE_6"].rect = {};
		m_sprites["USED_PALETTE_7"].rect = {};

	}

	// padding is 30, so 10 for each side + 10 inbewteen


}
}

