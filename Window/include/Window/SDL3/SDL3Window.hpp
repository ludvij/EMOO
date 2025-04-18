#ifndef GRAPHICS_WINDOW_SDL_WINDOW_HEADER
#define GRAPHICS_WINDOW_SDL_WINDOW_HEADER

#include "../Window.hpp"
#include <any>
#include <string>
#include <string_view>

struct SDL_Window;
namespace Window
{
class SDL3Window : public IWindow
{
	using NativeType = SDL_Window;
public:
	SDL3Window(const std::string& title, int w, int h);
	virtual ~SDL3Window() override;

	virtual WindowExtent GetDimensions() const override;
	virtual void GetDimensions(int* w, int* h) const override;

	virtual void* CreateRenderSurface(void* instance) override;

	virtual uint32_t GetWindowID() override;


	virtual void InitImguiForRenderer() override;

private:
	NativeType* m_window;

	// Inherited via IWindow
	void ShutdownImGuiWindow() override;

	// Inherited via IWindow
	void ProcessEventForImGui(void* event) override;

	// Inherited via IWindow
	void BeginImGuiFrame() override;

	// Inherited via IWindow
	void ProcessEvents(void* event) override;
	void Maximize(bool maximize) override;
};
}
#endif //!GRAPHICS_WINDOW_SDL_WINDOW_HEADER