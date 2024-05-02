#ifndef GRAPHICS_WINDOW_SDL_WINDOW_HEADER
#define  GRAPHICS_WINDOW_SDL_WINDOW_HEADER

#include "Window/Window.hpp"

struct SDL_Window;
namespace Ui
{
class SDLWindow : public IWindow
{
	using NativeType = SDL_Window;
public:
	SDLWindow(int w, int h);
	virtual ~SDLWindow() override;

	virtual WindowExtent GetDimensions() const override;
	virtual void GetDimensions(int* w, int* h) const override;

	virtual VkSurfaceKHR CreateVulkanSurface(VkInstance instance) override;

	virtual uint32_t GetWindowID() override;


	virtual void InitImguiForVulkan() override;

private:
	NativeType* m_window;
};
}
#endif //!GRAPHICS_WINDOW_SDL_WINDOW_HEADER