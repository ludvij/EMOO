#ifndef GRAPHICS_APPLICATION_HEADER
#define GRAPHICS_APPLICATION_HEADER

#include "Components/IComponent.hpp"
#include "Window/Window.hpp"
#include <functional>
#include <memory>
#include <NesEmu.hpp>
#include <string>

struct SDL_Window;

struct ImFont;


namespace Ui
{
struct Configuration
{
	std::string name = "TFG application";
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
	void shutdown();

	void main_loop();


private:
	bool m_should_quit = false;
	bool m_stop_rendering = false;
	Configuration m_config;

	Emu::Console m_console;

	std::vector<std::shared_ptr<IComponent>> m_components;
	std::function<void()> m_menuCallback;

	IWindow* m_window;

};

}

#endif

