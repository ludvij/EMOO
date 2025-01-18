#include "Application.hpp"

#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <print>
#include <ranges>
#include <vector>

#include <backends/imgui_impl_vulkan.h>
#include <SDL.h>

#include "Components/CloseDialog.hpp"
#include "Components/IComponent.hpp"
#include "Components/MemoryView.hpp"
#include "Components/ShowCPUStatus.hpp"
#include "Components/ShowPPUStatus.hpp"

#include <cppicons/IconsFontAwesome5.hpp>
#include <Input/SDL2/SDL2Input.hpp>
#include <pfd/portable_file_dialogs.h>
#include <RendererAPI.hpp>
#include <resources/fonts/CascadiaMono-Regular.embed>
#include <resources/fonts/fa-solid-900.embed>
#include <resources/fonts/OpenSans-Regular.embed>
#include <Window/SDL2/SDL2Window.hpp>	

#include <FileManager/FileManager.hpp>


#include <utils/Disassembler.hpp>

using namespace std::chrono_literals;
namespace stdc = std::chrono;
using us_t = stdc::microseconds;

namespace Ui
{
Application* s_instance;

Application::Application(const Configuration& config)
	: m_config(config)
	, m_console(Emu::NTSC)
{
	s_instance = this;
	m_window = std::make_shared<Window::SDL2Window>(m_config.name, m_config.w, m_config.h);
	m_input = std::make_unique<Input::SDL2Input>();

	init();

	const auto w = static_cast<uint32_t>( m_console.GetConfig().width );
	const auto h = static_cast<uint32_t>( m_console.GetConfig().height );

	m_screen = Renderer::CreateTexture(w, h, Renderer::TextureType::BINDLESS);

	m_screen_sprite = Renderer::Sprite({}, 0, m_screen);
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

double Application::GetDelta() const
{
	return m_delta;
}

void Application::Error(const char* name, std::string_view msg)
{
	AddComponent<Component::CloseDialog>(name, msg, false);
}

Emu::Console& Application::GetConsole()
{
	return m_console;
}

void Application::SetUpdate(bool set)
{
	m_can_update = set;
}

void Application::init()
{
	Renderer::Init(m_window, true);
	auto& io = ImGui::GetIO();
	// set app directory to appdata/roaming/EMOO
	Fman::PushFolder("EMOO");
	Fman::SetRoot();

	io.IniFilename = Fman::AllocateFileName("imgui.ini");

	const float base_font_size = 20.0f;
	const float icon_font_size = base_font_size * 2.0f / 3.0f;
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };

	ImFontConfig font_config;
	font_config.FontDataOwnedByAtlas = true;

	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMaxAdvanceX = icon_font_size;

	m_monospace_font = io.Fonts->AddFontFromMemoryCompressedTTF(
		CascadiaMono_compressed_data,
		CascadiaMono_compressed_size,
		base_font_size,
		&font_config
	);
	io.Fonts->AddFontFromMemoryCompressedTTF(
		fa_solid_900_compressed_data,
		fa_solid_900_compressed_size,
		icon_font_size,
		&icons_config,
		icons_ranges
	);
	ImFont* default_font = io.Fonts->AddFontFromMemoryCompressedTTF(
		OpenSans_compressed_data,
		OpenSans_compressed_size,
		base_font_size,
		&font_config
	);
	io.Fonts->AddFontFromMemoryCompressedTTF(
		fa_solid_900_compressed_data,
		fa_solid_900_compressed_size,
		icon_font_size,
		&icons_config,
		icons_ranges
	);
	io.FontDefault = default_font;
	Renderer::BuildFontTexture();
	init_button_actions();
	init_keyboard_actions();
	init_windowevent_actions();
}

void Application::init_button_actions()
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
			m_console.GetController(0).SetPressed(Emu::Button::UP);
		};
	auto press_down = [&](Input::IInput* i)
		{
			m_console.GetController(0).SetPressed(Emu::Button::DOWN);
		};
	auto press_left = [&](Input::IInput* i)
		{
			m_console.GetController(0).SetPressed(Emu::Button::LEFT);
		};
	auto press_right = [&](Input::IInput* i)
		{
			m_console.GetController(0).SetPressed(Emu::Button::RIGHT);
		};
	auto press_start = [&](Input::IInput* i)
		{
			m_console.GetController(0).SetPressed(Emu::Button::START);
		};
	auto press_select = [&](Input::IInput* i)
		{
			m_console.GetController(0).SetPressed(Emu::Button::SELECT);
		};
	m_input->AddAction(Input::Button::FACE_DOWN, press_a);
	m_input->AddAction(Input::Button::FACE_RIGHT, press_a);
	m_input->AddAction(Input::Button::FACE_LEFT, press_b);
	m_input->AddAction(Input::Button::FACE_UP, press_b);
	m_input->AddAction(Input::Button::DPAD_UP, press_up);
	m_input->AddAction(Input::Button::DPAD_DOWN, press_down);
	m_input->AddAction(Input::Button::DPAD_LEFT, press_left);
	m_input->AddAction(Input::Button::DPAD_RIGHT, press_right);
	m_input->AddAction(Input::Button::START, press_start);
	m_input->AddAction(Input::Button::SELECT, press_select);

}

void Application::init_keyboard_actions()
{
	typedef Input::Key K;
	auto action_stop_continue = [&](Input::IInput* i)
		{
			INPUT_NOT_REPEATED(i);

			m_emulation_stopped = !m_emulation_stopped;
		};

	auto action_run_cpu_instructin = [&](Input::IInput* i)
		{
			INPUT_REPEAT_AFTER(i, 1000ms);
			INPUT_REPEAT_EVERY(i, 50ms);
			INPUT_KEY_NOT_MODIFIED(i);

			run_cpu_instruction();
		};
	auto action_run_frame = [&](Input::IInput* i)
		{
			INPUT_REPEAT_AFTER(i, 1000ms);
			INPUT_REPEAT_EVERY(i, 50ms);
			INPUT_KEY_MODIFIED(i, K::SHIFT);

			run_frame();
		};
	auto action_run_pixel = [&](Input::IInput* i)
		{
			INPUT_REPEAT_AFTER(i, 1000ms);
			INPUT_REPEAT_EVERY(i, 10ms);
			INPUT_KEY_MODIFIED(i, K::SHIFT);

			run_pixel();
		};

	auto action_exit = [&](Input::IInput* i)
		{
			INPUT_NOT_REPEATED(i);
			INPUT_KEY_MODIFIED(i, K::SHIFT);

			Close();
		};
	auto action_reset = [&](Input::IInput* i)
		{
			INPUT_NOT_REPEATED(i);
			INPUT_KEY_MODIFIED(i, K::SHIFT);
			m_console.Reset();
		};

	auto action_run_ppu_cycle = [&](Input::IInput* i)
		{
			INPUT_REPEAT_AFTER(i, 1000ms);
			INPUT_REPEAT_EVERY(i, 10ms);
			INPUT_KEY_MODIFIED(i, K::CTRL);

			run_ppu_cycle();
		};
	auto action_run_cpu_cycle = [&](Input::IInput* i)
		{
			INPUT_REPEAT_AFTER(i, 1000ms);
			INPUT_REPEAT_EVERY(i, 10ms);
			INPUT_KEY_MODIFIED(i, K::CTRL);

			run_cpu_cycle();
		};

	auto action_run_scanline = [&](Input::IInput* i)
		{
			INPUT_REPEAT_AFTER(i, 1000ms);
			INPUT_REPEAT_EVERY(i, 10ms);
			INPUT_KEY_NOT_MODIFIED(i);

			run_scanline();
		};
	auto action_load_rom = [&](Input::IInput* i)
		{
			INPUT_NOT_REPEATED(i);
			INPUT_KEY_MODIFIED(i, K::CTRL);

			load_rom();
		};
	auto action_cpu_status = [&](Input::IInput* i)
		{
			INPUT_NOT_REPEATED(i);
			INPUT_KEY_MODIFIED(i, K::CTRL);

			cpu_status();
		};
	auto action_ppu_status = [&](Input::IInput* i)
		{
			INPUT_NOT_REPEATED(i);
			INPUT_KEY_MODIFIED(i, K::CTRL);

			ppu_status();
		};
	auto action_memory_view = [&](Input::IInput* i)
		{
			INPUT_NOT_REPEATED(i);
			INPUT_KEY_MODIFIED(i, K::LCTRL, K::RCTRL);

			memory_view();
		};
	auto action_save_state = [&](Input::IInput* i)
		{
			INPUT_NOT_REPEATED(i);
			INPUT_KEY_NOT_MODIFIED(i);
			save_state();
		};
	auto action_load_state = [&](Input::IInput* i)
		{
			INPUT_NOT_REPEATED(i);
			INPUT_KEY_MODIFIED(i, K::SHIFT);
			load_state();
		};
	auto action_increment_state = [&](Input::IInput* i)
		{
			INPUT_NOT_REPEATED(i);
			INPUT_KEY_NOT_MODIFIED(i);
			increment_state();
		};
	auto action_decrement_state = [&](Input::IInput* i)
		{
			INPUT_NOT_REPEATED(i);
			INPUT_KEY_NOT_MODIFIED(i);
			decrement_state();
		};
	auto action_maximize_window = [&](Input::IInput* i)
		{
			INPUT_NOT_REPEATED(i);
			INPUT_KEY_MODIFIED(i, K::LALT, K::RALT);
			m_window->Maximize(true);
		};
	m_input->AddAction(K::F9, action_stop_continue);
	m_input->AddAction(K::F9, action_run_frame);
	m_input->AddAction(K::F10, action_run_scanline);
	m_input->AddAction(K::F10, action_run_pixel);
	m_input->AddAction(K::F10, action_run_ppu_cycle);
	m_input->AddAction(K::F11, action_run_cpu_instructin);
	m_input->AddAction(K::F11, action_run_cpu_cycle);
	m_input->AddAction(K::F8, action_reset);
	m_input->AddAction(K::ESCAPE, action_exit);
	m_input->AddAction(K::O, action_load_rom);
	m_input->AddAction(K::C, action_cpu_status);
	m_input->AddAction(K::P, action_ppu_status);
	m_input->AddAction(K::M, action_memory_view);

	m_input->AddAction(K::F5, action_save_state);
	m_input->AddAction(K::F5, action_load_state);
	m_input->AddAction(K::F1, action_increment_state);
	m_input->AddAction(K::F2, action_decrement_state);

	m_input->AddAction(K::ENTER, action_maximize_window);

}

void Application::init_windowevent_actions()
{
	typedef Window::Event E;
	m_window->AddEventFunction(E::RESIZED, [&](Window::IWindow* i, void* event)
		{
			Renderer::Resize();
			m_resized = true;
		});
	m_window->AddEventFunction(E::MINIMIZED, [&](Window::IWindow* i, void* event)
		{
			m_stop_rendering = true;
			m_minimized = true;
		});
	m_window->AddEventFunction(E::RESTORED, [&](Window::IWindow* i, void* event)
		{
			m_stop_rendering = false;
			m_minimized = false;
		});
	m_window->AddEventFunction(E::CLOSE, [&](Window::IWindow* i, void* event)
		{
			auto e = static_cast<SDL_Event*>( event );
			if (e->window.windowID == i->GetWindowID())
			{
				m_should_quit = true;
			}
		});
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

Renderer::Rect Application::GetEmuRect() const
{
	return m_screen_sprite.rect;
}

void Application::run_cpu_instruction()
{
	m_emulation_stopped = true;

	m_console.RunCpuInstruction();
}

void Application::run_frame()
{
	m_emulation_stopped = true;
	m_console.RunFrame();
}

void Application::run_pixel()
{
	m_emulation_stopped = true;
	m_console.RunPpuPixel();


}

void Application::run_scanline()
{
	m_emulation_stopped = true;
	m_console.RunPpuScanline();
}

void Application::run_ppu_cycle()
{
	m_emulation_stopped = true;
	m_console.RunPpuCycle();
}

void Application::run_cpu_cycle()
{
	m_emulation_stopped = true;
	m_console.RunCpuCycle();
}

void Application::load_rom()
{
	if (!m_can_update)
	{
		return;
	}
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

void Application::ppu_status()
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

void Application::cpu_status()
{
	if (m_components.contains("cpu status"))
	{
		RemoveComponent("cpu status");
	}
	else
	{
		AddComponent<Component::ShowCPUStatus>("cpu status", m_monospace_font);
	}
}

void Application::memory_view()
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

void Application::save_state()
{
	m_console.SaveState(m_current_state);
}

void Application::load_state()
{
	m_console.LoadState(m_current_state);
}

void Application::increment_state()
{
	m_current_state++;
	if (m_current_state > MAX_STATE_AMOUNT)
	{
		m_current_state = 0;
	}
}

void Application::decrement_state()
{
	m_current_state--;
	if (m_current_state < 0)
	{
		m_current_state = MAX_STATE_AMOUNT;
	}
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
ImVec2 Application::GetScreenSize()
{
	return { m_screen_w, m_screen_h };
}
void Application::main_loop()
{
	while (!m_should_quit)
	{
		auto begin = stdc::high_resolution_clock::now();
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

		auto end = stdc::high_resolution_clock::now();
		auto duration = stdc::duration_cast<us_t>( end - begin );
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
		case SDL_QUIT:
			m_should_quit = true;
			break;
		default:
			break;
		}
		m_window->ProcessEvents(&event);
		m_input->ProcessEvents(&event);
	}

	m_input->Update();

}

void Application::draw_ui()
{
	Renderer::BeginImGuiFrame();
	m_window->BeginImGuiFrame();

	ImGui::NewFrame();
	try
	{
		draw_menu_bar();
		draw_dockspace();


		for (const auto& [k, v] : m_components)
		{
			v->OnRender();
		}
	} catch (const std::exception&)
	{
		m_components.clear();
		AddComponent<Component::CloseDialog>("oopsie", "Unrecoverable error, sorry :&");
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
			if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Load ROM", "Ctrl+O") && m_can_update)
			{
				load_rom();
			}
			if (ImGui::MenuItem(ICON_FA_REDO " Reset", "Shift+F8"))
			{
				m_console.Reset();
			}
			if (ImGui::MenuItem(ICON_FA_TIMES" Exit", "Shift+Esc"))
			{
				Close();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Emulation"))
		{
			if (m_emulation_stopped && ImGui::MenuItem(ICON_FA_PLAY " Run", "F9"))
			{
				m_emulation_stopped = false;
			}
			else if (!m_emulation_stopped && ImGui::MenuItem(ICON_FA_PAUSE " Stop", "F9"))
			{
				m_emulation_stopped = true;
			}
			if (ImGui::MenuItem("Run Frame", "Shift+F9"))
			{
				run_frame();
			}
			if (ImGui::BeginMenu("PPU"))
			{
				if (ImGui::MenuItem("Run Scanline", "F10"))
				{
					run_scanline();
				}
				if (ImGui::MenuItem("Run pixel", "Shift+F10"))
				{
					run_pixel();
				}
				if (ImGui::MenuItem("Run Cycle", "Ctrl+F10"))
				{
					run_ppu_cycle();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("CPU"))
			{
				if (ImGui::MenuItem("Run Instruction", "F11"))
				{
					run_cpu_instruction();
				}
				if (ImGui::MenuItem("Run Cycle", "Ctrl+F11"))
				{
					run_cpu_cycle();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem(ICON_FA_IMAGE " PPU status", "Ctrl+P"))
			{
				ppu_status();
			}
			if (ImGui::MenuItem(ICON_FA_MICROCHIP " CPU status", "Ctrl+C"))
			{
				cpu_status();
			}
			if (ImGui::MenuItem(ICON_FA_DATABASE " Memory status", "Ctrl+M"))
			{
				memory_view();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu(std::format("state [{:d}]", m_current_state).c_str()))
		{
			if (ImGui::MenuItem("Save state", "F5"))
			{
				save_state();
			}
			if (ImGui::MenuItem("Load state", "Shift+F5"))
			{
				load_state();
			}
			if (ImGui::MenuItem("Increment state", "F1"))
			{
				increment_state();
			}
			if (ImGui::MenuItem("Decrement state", "F2"))
			{
				decrement_state();
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
	if (!m_emulation_stopped && !m_minimized)
	{
		try
		{
			m_console.RunFrame();
			// in case of STP
		} catch (const std::runtime_error&)
		{
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
	m_screen->SetData(m_console.GetPpu().GetScreen());
}
void Application::resize_emu_screen()
{
	const auto [window_width, window_height] = m_window->GetDimensions();
	const float w = static_cast<float>( window_width );
	// taking into account menu bar
	const float h = static_cast<float>( window_height - m_menu_bar_height );

	const float nes_aspect_ratio = m_console.GetConfig().width / m_console.GetConfig().height;

	Renderer::Rect new_bounds;
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

