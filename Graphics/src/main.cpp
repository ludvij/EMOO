
#include "Application.hpp"
#include <iostream>

using Ui::Application;

namespace Ui
{
int Main(int argc, char** argv)
{
	Application* application = new Application();

	application->Run();

	delete application;

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