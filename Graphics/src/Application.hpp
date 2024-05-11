#ifndef GRAPHICS_APPLICATION_HEADER
#define GRAPHICS_APPLICATION_HEADER

#include "Components/IComponent.hpp"
#include "Input/Input.hpp"
#include "Renderer/Sprite.hpp"
#include "Window/Window.hpp"

#include <functional>
#include <memory>
#include <NesEmu.hpp>
#include <string>

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

	Emu::Console& GetConsole()
	{
		return m_console;
	}

	void SetMenuCallback(const std::function<void()>& menubar_callback)
	{
		m_menuCallback = menubar_callback;
	}

	template<typename T>
	void AddComponent() requires( std::derived_from<T, IComponent> )
	{
		m_components.emplace_back(std::make_shared<T>())->OnCreate();
	}

	void Run();
	void Close();

private:
	void init();
	void init_button_mapping();
	void shutdown();

	void main_loop();

	void event_loop();

	void draw_ui();
	void draw_application();
	void update();

	void get_pixel_data();


	void resize_emu_screen();
	void resize_other();


private:
	bool m_should_quit{ false };
	bool m_stop_rendering{ false };
	Configuration m_config;

	Emu::Console m_console;

	std::vector<std::shared_ptr<IComponent>> m_components;
	std::function<void()> m_menuCallback;
	std::unordered_map<const char*, std::unique_ptr<ITexture>> m_textures;
	std::unordered_map<const char*, Sprite> m_sprites;

	std::unique_ptr<IInput> m_input;
	std::shared_ptr<IWindow> m_window;

	bool m_resized{ true };

	float m_menu_bar_height{ 26 };

	uint32_t m_frame_rate{};

};

}

#endif

