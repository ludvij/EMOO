#ifndef GRAPHICS_APPLICATION_HEADER
#define GRAPHICS_APPLICATION_HEADER

#include "Components/IComponent.hpp"
#include <Input/Input.hpp>
#include <Sprite.hpp>
#include <Window/Window.hpp>

#include <functional>
#include <memory>
#include <NesEmu.hpp>
#include <string>

#include <unordered_map>
#include <utils/Disassembler.hpp>

struct ImFont;
struct SDL_Window;



namespace Ui
{
struct Configuration
{
	std::string name = "EMOO";
	uint32_t w = 1600;
	uint32_t h = 900;
};



class Application
{
public:
	Application(const Configuration& config = Configuration());
	~Application();

	static Application& Get();

	static double GetDelta();

	void Error(const char* name, std::string_view msg);

	static Emu::Console& GetConsole();

	static void SetUpdate(bool set);

	template<typename T, class... Args>
	void AddComponent(const std::string_view name, Args... args) requires( std::derived_from<T, Component::IComponent> )
	{
		std::shared_ptr<T> comp = std::make_shared<T>(name, std::forward<Args>(args)...);
		comp->OnCreate();
		m_components.insert({ comp->name, comp });
	}

	void AddComponent(const std::shared_ptr<Component::IComponent>& component);

	void RemoveComponent(const std::string_view name);

	void RunCpuInstruction();
	void RunFrame();
	void RunPixel();
	void RunScanline();
	void RunPpuCycle();
	void RunCpuCycle();

	void Run();
	void Close();

	void RestartEmulator();
private:
	void init();
	void init_button_mapping();
	void init_keyboard_actions();
	void shutdown();

	void main_loop();

	void event_loop();

	void draw_ui();
	void draw_dockspace();
	// can't offload this to an component without making 5 quintillion callbacks
	void draw_menu_bar();
	void draw_application();
	void update();

	void clear_deleted_components();

	void get_pixel_data();


	void resize_emu_screen();


private:
	bool m_should_quit{ false };
	bool m_stop_rendering{ false };
	Configuration m_config;

	Emu::Console m_console;

	std::unordered_map<std::string_view, std::shared_ptr<Component::IComponent>> m_components;
	// this only exists so I can forbid recreation of some components

	ITexture* m_screen;
	Sprite m_screen_sprite;

	std::unique_ptr<Input::IInput> m_input;
	std::shared_ptr<Window::IWindow> m_window;


	float m_menu_bar_height{ 26 };

	float m_avail_x{ 0 };
	float m_avail_y{ 0 };

	float m_screen_w{ 0 };
	float m_screen_h{ 0 };

	bool m_resized{ true };
	bool m_can_update{ true };
	bool m_emulation_stopped{ false };

	ImFont* m_monospace_font{ nullptr };

	double m_delta{};
};

}

#endif

