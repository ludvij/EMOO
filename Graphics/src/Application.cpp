#include "Application.hpp"

#include <functional>
#include <iostream>
#include <print>
#include <ranges>
#include <vector>

#include <backends/imgui_impl_vulkan.h>
#include <SDL3/SDL.h>

#include "Components/CloseDialog.hpp"
#include "Components/IComponent.hpp"
#include "Components/MemoryView.hpp"
#include "Components/ShowCPUStatus.hpp"
#include "Components/ShowPPUStatus.hpp"

#include "Renderer/RendererAPI.hpp"
#include "Window/SDL/SDLWindow.hpp"	
#include <SDL/SDLInput.hpp>

#include "ImGui/CascadiaMono-Regular.embed"
#include "Renderer/Vulkan/VulkanTexture.hpp"
#include <pfd/portable_file_dialogs.h>

#include <memory>

#include <numeric>

#include <lud_id.hpp>

#include <utils/Disassembler.hpp>

using namespace std::chrono_literals;

namespace Ui
{
Application* s_instance;

Application::Application(const Configuration& config)
	: m_config(config)
	, m_console(Emu::NTSC)
{
	s_instance = this;
	m_window = std::make_shared<SDLWindow>(m_config.name, m_config.w, m_config.h);
	m_input = std::make_unique<Input::SDLInput>();
	init_button_mapping();
	init_keyboard_actions();
	init();


	const auto w = static_cast<uint32_t>( m_console.GetConfig().width );
	const auto h = static_cast<uint32_t>( m_console.GetConfig().height );

	m_screen = Renderer::CreateBindlessTexture(w, h);

	m_screen_sprite = Sprite({}, 0, m_screen);
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

double Application::GetDelta()
{
	return Get().m_delta;
}

void Application::Error(const char* name, std::string_view msg)
{
	AddComponent<Component::CloseDialog>(name, msg, false);
}

Emu::Console& Application::GetConsole()
{
	return Get().m_console;
}

void Application::SetUpdate(bool set)
{
	Get().m_can_update = set;
}

void Application::init()
{
	Renderer::Init(m_window, true);
	m_monospace_font = Renderer::GetMonospaceFont();
}

void Application::init_button_mapping()
{
	auto press_a = [&](Input::IInput* i)
		{
			m_console.GetController(0).SetPressed(Emu::Button::A);
		};
	auto press_b = [&](Input::IInput* i)
		{
			m_console.GetController(0).SetPressed(Emu::Button::B);
		};
	auto press_up = [&](Input::IInput* i)
		{
			m_console.GetController(0).SetPressed(Emu::Button::Up);
		};
	auto press_down = [&](Input::IInput* i)
		{
			m_console.GetController(0).SetPressed(Emu::Button::Down);
		};
	auto press_left = [&](Input::IInput* i)
		{
			m_console.GetController(0).SetPressed(Emu::Button::Left);
		};
	auto press_right = [&](Input::IInput* i)
		{
			m_console.GetController(0).SetPressed(Emu::Button::Right);
		};
	auto press_start = [&](Input::IInput* i)
		{
			m_console.GetController(0).SetPressed(Emu::Button::Start);
		};
	auto press_select = [&](Input::IInput* i)
		{
			m_console.GetController(0).SetPressed(Emu::Button::Select);
		};
	m_input->AddGamepadAction(Input::Button::FACE_DOWN, press_a);
	m_input->AddGamepadAction(Input::Button::FACE_LEFT, press_b);
	m_input->AddGamepadAction(Input::Button::FACE_RIGHT, press_a);
	m_input->AddGamepadAction(Input::Button::FACE_UP, press_b);
	m_input->AddGamepadAction(Input::Button::DPAD_UP, press_up);
	m_input->AddGamepadAction(Input::Button::DPAD_DOWN, press_down);
	m_input->AddGamepadAction(Input::Button::DPAD_LEFT, press_left);
	m_input->AddGamepadAction(Input::Button::DPAD_RIGHT, press_right);
	m_input->AddGamepadAction(Input::Button::START, press_start);
	m_input->AddGamepadAction(Input::Button::SELECT, press_select);

}

void Application::init_keyboard_actions()
{
	auto stop_continue = [&](Input::IInput* i)
		{
			INPUT_NOT_REPEATED(i);

			m_emulation_stopped = !m_emulation_stopped;
		};

	auto run_cpu_instructin = [&](Input::IInput* i)
		{
			INPUT_REPEAT_AFTER(i, 1000ms);
			INPUT_REPEAT_EVERY(i, 50ms);
			INPUT_KEY_NOT_MODIFIED(i);

			RunCpuInstruction();
		};
	auto run_frame = [&](Input::IInput* i)
		{
			INPUT_REPEAT_AFTER(i, 1000ms);
			INPUT_REPEAT_EVERY(i, 50ms);
			INPUT_KEY_MODIFIED(i, Input::Key::RSHIFT, Input::Key::LSHIFT);

			RunFrame();
		};
	auto run_pixel = [&](Input::IInput* i)
		{
			INPUT_REPEAT_AFTER(i, 1000ms);
			INPUT_REPEAT_EVERY(i, 10ms);
			INPUT_KEY_MODIFIED(i, Input::Key::RSHIFT, Input::Key::LSHIFT);

			RunPixel();
		};

	auto exit = [&](Input::IInput* i)
		{
			INPUT_NOT_REPEATED(i);
			INPUT_KEY_MODIFIED(i, Input::Key::RSHIFT, Input::Key::LSHIFT);

			Close();
		};
	auto reset = [&](Input::IInput* i)
		{
			INPUT_NOT_REPEATED(i);
			INPUT_KEY_MODIFIED(i, Input::Key::RSHIFT, Input::Key::LSHIFT);
			m_console.Reset();
		};

	auto run_ppu_cycle = [&](Input::IInput* i)
		{
			INPUT_REPEAT_AFTER(i, 1000ms);
			INPUT_REPEAT_EVERY(i, 10ms);
			INPUT_KEY_MODIFIED(i, Input::Key::RCTRL, Input::Key::LCTRL);

			RunPpuCycle();
		};
	auto run_cpu_cycle = [&](Input::IInput* i)
		{
			INPUT_REPEAT_AFTER(i, 1000ms);
			INPUT_REPEAT_EVERY(i, 10ms);
			INPUT_KEY_MODIFIED(i, Input::Key::RCTRL, Input::Key::LCTRL);

			RunCpuCycle();
		};

	auto run_scanline = [&](Input::IInput* i)
		{
			INPUT_REPEAT_AFTER(i, 1000ms);
			INPUT_REPEAT_EVERY(i, 10ms);
			INPUT_KEY_NOT_MODIFIED(i);

			RunScanline();
		};
	m_input->AddKeyboardAction(Input::Key::F9, stop_continue);
	m_input->AddKeyboardAction(Input::Key::F9, run_frame);

	m_input->AddKeyboardAction(Input::Key::F10, run_scanline);
	m_input->AddKeyboardAction(Input::Key::F10, run_pixel);
	m_input->AddKeyboardAction(Input::Key::F10, run_ppu_cycle);

	m_input->AddKeyboardAction(Input::Key::F11, run_cpu_instructin);
	m_input->AddKeyboardAction(Input::Key::F11, run_cpu_cycle);

	m_input->AddKeyboardAction(Input::Key::F8, reset);
	m_input->AddKeyboardAction(Input::Key::ESCAPE, exit);
}

void Application::shutdown()
{
	m_components.clear();
	delete m_screen;
}


void Application::AddComponent(const std::shared_ptr<Component::IComponent>& component)
{
	m_components.insert({ component->name, component });
	component->OnCreate();
}

void Application::RemoveComponent(const std::string_view id)
{
	m_components[id]->removed = true;
}

void Application::RunCpuInstruction()
{
	m_emulation_stopped = true;

	m_console.RunCpuInstruction();
}

void Application::RunFrame()
{
	m_emulation_stopped = true;
	m_console.RunFrame();
}

void Application::RunPixel()
{
	m_emulation_stopped = true;
	m_console.RunPpuPixel();


}

void Application::RunScanline()
{
	m_emulation_stopped = true;
	m_console.RunPpuScanline();
}

void Application::RunPpuCycle()
{
	m_emulation_stopped = true;
	m_console.RunPpuCycle();
}

void Application::RunCpuCycle()
{
	m_emulation_stopped = true;
	m_console.RunCpuCycle();
}

void Application::Run()
{
	main_loop();
}

void Application::Close()
{
	m_should_quit = true;
}
void Application::RestartEmulator()
{
	//m_console = Emu::Console(Emu::NTSC);
}
void Application::main_loop()
{
	while (!m_should_quit)
	{
		auto begin = std::chrono::high_resolution_clock::now();
		event_loop();

		if (m_stop_rendering)
		{
			continue;
		}
		Renderer::Resize();
		if (m_can_update)
		{
			update();
		}

		clear_deleted_components();
		draw_application();

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>( end - begin );
		m_delta = static_cast<double>( duration.count() ) / 1e6;
	}
}
void Application::event_loop()
{
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		switch (event.type)
		{
		case SDL_EVENT_QUIT:
			m_should_quit = true;
			break;
		case SDL_EVENT_WINDOW_RESIZED:
			Renderer::RequestResize();
			m_resized = true;
			break;
		case SDL_EVENT_WINDOW_MINIMIZED:
			m_stop_rendering = true;
			break;
		case SDL_EVENT_WINDOW_RESTORED:
			m_stop_rendering = false;
			break;
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			if (event.window.windowID == m_window->GetWindowID())
			{
				m_should_quit = true;
			}
			break;
		default:
			break;
		}
		m_window->ProcessEventForImGui(&event);
		m_input->ProcessEvents(&event);
	}

	m_input->Update();

}

void Application::draw_ui()
{
	m_window->BeginImGuiFrame();
	Renderer::BeginImGuiFrame();

	ImGui::NewFrame();
	draw_menu_bar();
	draw_dockspace();

	for (const auto& [k, v] : m_components)
	{
		v->OnRender();
	}

	ImGui::End();


	ImGui::Render();
	// Update and Render additional Platform Windows
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}
void Application::draw_dockspace()
{
	static ImGuiDockNodeFlags d_flags = ImGuiDockNodeFlags_None;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos({ viewport->WorkPos.x + m_screen_w, viewport->WorkPos.y });
	ImGui::SetNextWindowSize({ m_avail_x, viewport->WorkSize.y });
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGuiWindowFlags w_flags =
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;
	if (d_flags & ImGuiDockNodeFlags_PassthruCentralNode)
	{
		w_flags |= ImGuiWindowFlags_NoBackground;
	}
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
	ImGui::Begin("Dockspace", nullptr, w_flags);
	ImGui::PopStyleVar();
	ImGui::PopStyleVar(2);

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID d_id = ImGui::GetID("Application_dockspace");
		ImGui::DockSpace(d_id, { 0, 0 }, d_flags);
	}

}
void Application::draw_menu_bar()
{
	if (ImGui::BeginMainMenuBar())
	{
		m_menu_bar_height = ImGui::GetWindowSize().y;
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Load ROM") && m_can_update)
			{
				// TODO:
				auto f = pfd::open_file("Chose ROM File", pfd::path::home(),
					{
						"Rom Files (.nes, .ines)", "*.nes *.ines",
						"All files", "*"
					});

				if (!f.result().empty())
				{
					try
					{
						m_console.LoadCartridge(f.result()[0]);

					} catch (const std::runtime_error&)
					{
						AddComponent<Component::CloseDialog>("close on file error", "Not a valid ROM", false);
					}
				}
			}
			if (ImGui::MenuItem("Reset (shift + F8)"))
			{
				m_console.Reset();
			}
			if (ImGui::MenuItem("Exit (shift + Esc)"))
			{
				Close();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Emulation"))
		{
			if (m_emulation_stopped && ImGui::MenuItem("Run (F9)"))
			{
				m_emulation_stopped = false;
			}
			else if (!m_emulation_stopped && ImGui::MenuItem("Stop (F9)"))
			{
				m_emulation_stopped = true;
			}
			if (ImGui::MenuItem("Run Frame (shift + F9)"))
			{
				RunFrame();
			}
			if (ImGui::BeginMenu("PPU"))
			{
				if (ImGui::MenuItem("Run Scanline (F10)"))
				{
					RunScanline();
				}
				if (ImGui::MenuItem("Run pixel (shift + F10)"))
				{
					RunPixel();
				}
				if (ImGui::MenuItem("Run Cycle (ctrl + F10"))
				{
					RunPpuCycle();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("CPU"))
			{
				if (ImGui::MenuItem("Run Instruction (F11)"))
				{
					RunCpuInstruction();
				}
				if (ImGui::MenuItem("Run Cycle (ctrl + F11)"))
				{
					RunCpuCycle();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Status"))
		{
			if (ImGui::MenuItem("Show PPU status"))
			{
				if (m_components.contains("ppu status"))
				{
					RemoveComponent("ppu status");
				}
				else
				{
					AddComponent<Component::ShowPPUStatus>("ppu status", m_monospace_font);
				}
			}
			if (ImGui::MenuItem("Show CPU status"))
			{
				if (m_components.contains("cpu status"))
				{
					RemoveComponent("cpu status");
				}
				else
				{
					AddComponent<Component::ShowCPUStatus>("cpu status");
				}
			}
			if (ImGui::MenuItem("View memory"))
			{
				if (m_components.contains("memory view"))
				{
					RemoveComponent("memory view");
				}
				else
				{
					AddComponent<Component::MemoryView>("memory view", m_monospace_font);
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}
void Application::draw_application()
{
	draw_ui();

	m_screen_sprite.Draw();

	Renderer::Draw();
}
void Application::update()
{
	if (!m_emulation_stopped)
	{
		try
		{
			m_console.RunFrame();
			// in case of STP
		} catch (const std::runtime_error&)
		{
			m_can_update = false;
			AddComponent<Component::CloseDialog>("close on stp", "STP opcode was executed");
		}
	}
	get_pixel_data();

	for (const auto& [k, v] : m_components)
	{
		v->OnUpdate();
	}

	if (m_resized)
	{
		m_resized = false;
		resize_emu_screen();
	}
}
// TODO: make better
void Application::clear_deleted_components()
{
	std::list<std::string_view> deleter;
	for (const auto& [k, v] : m_components)
	{
		if (v->removed)
		{
			deleter.push_back(k);
		}
	}

	for (const auto& del : deleter)
	{
		m_components.erase(del);
	}
}

void Application::get_pixel_data()
{
	m_screen->SetData(m_console.OutputScreen());
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

		m_avail_x = w - new_bounds.w;
		m_avail_y = h;
	}
	else
	{
		new_bounds.w = w;
		new_bounds.h = w / nes_aspect_ratio;

		m_avail_x = w;
		m_avail_y = h - new_bounds.h;
	}
	new_bounds.x = 0;
	new_bounds.y = m_menu_bar_height;

	m_screen_h = new_bounds.h;
	m_screen_w = new_bounds.w;

	m_screen_sprite.rect = new_bounds;


}
}

