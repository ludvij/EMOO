
#include "Application.hpp"
#include <print>

#include <SDL.h>

using Ui::Application;

#include <FileManager/FileManager.hpp>

namespace Ui
{
int Main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	// windows only code to make scale no mess up the window size
	SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "system");

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

	Application* application = new Application();
	application->Run();

	delete application;
	SDL_Quit();

	return 0;
}
}


#if defined(GRAPHICS_NDEBUG) && defined(NES_EMU_PLATFORM_WINDOWS)

#include <Windows.h>

int APIENTRY WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hInstPrev, _In_ PSTR cmdline, _In_ int cmdshow)
{
	return Ui::Main(__argc, __argv);
}

#else


int main(int argc, char** argv)
{
	return Ui::Main(argc, argv);
}

#endif