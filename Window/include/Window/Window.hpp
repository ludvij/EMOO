#ifndef GRAPHICS_WINDOW_HEADER
#define GRAPHICS_WINDOW_HEADER

#include <functional>
#include <unordered_map>
#include <vulkan/vulkan.h>

namespace Window
{

struct WindowExtent
{
	int w;
	int h;
};

enum class Event
{
	NONE,
	RESIZED,
	MINIMIZED,
	RESTORED,
	MAXIMIZED,
	CLOSE,
	MOVED,
	FOCUS_LOST_MOUSE,
	FOCUS_GAIN_MOUSE,
	FOCUS_LOST_KEYBOARD,
	FOCUS_GAIN_KEYBOARD,
};

class IWindow
{
public:
	virtual ~IWindow() = default;

	virtual WindowExtent GetDimensions() const = 0;
	virtual void GetDimensions(int* w, int* h) const = 0;

	virtual void* CreateRenderSurface(void* instance) = 0;

	virtual uint32_t GetWindowID() = 0;

	// imgui functions
	virtual void InitImguiForRenderer() = 0;
	virtual void ShutdownImGuiWindow() = 0;

	virtual void BeginImGuiFrame() = 0;

	void AddEventFunction(Event e, const std::function<void(IWindow*, void*)>&& fn);

	virtual void ProcessEvents(void* event) = 0;

	virtual void Maximize(bool maximize) = 0;


protected:
	std::unordered_map <Event, std::function<void(IWindow*, void*)>> m_event_fn;
};
}

#endif