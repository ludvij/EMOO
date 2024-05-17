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

	static void SetUpdate(bool set);

	Emu::Console& GetConsole()
	{
		return m_console;
	}

	template<typename T, class... Args>
	void AddComponent(Args... args) requires( std::derived_from<T, Component::IComponent> )
	{
		m_components.emplace_back(std::make_shared<T>(std::forward<Args>(args)...))->OnCreate();
	}

	void AddComponent(const std::shared_ptr<Component::IComponent>& component)
	{
		m_components.emplace_back(component);
		component->OnCreate();
	}

	void Run();
	void Close();

	void RestartEmulator();
private:
	void init();
	void init_button_mapping();
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

	std::list<std::shared_ptr<Component::IComponent>> m_components;

	std::unordered_map<const char*, std::unique_ptr<ITexture>> m_textures;
	std::unordered_map<const char*, Sprite> m_sprites;

	std::unique_ptr<IInput> m_input;
	std::shared_ptr<IWindow> m_window;

	bool m_resized{ true };

	float m_menu_bar_height{ 26 };
	bool m_can_update{ true };

	float m_avail_x{ 0 };
	float m_avail_y{ 0 };

	float m_screen_w{ 0 };
	float m_screen_h{ 0 };
};

}

#endif

