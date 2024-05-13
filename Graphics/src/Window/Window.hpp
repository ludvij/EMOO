#ifndef GRAPHICS_WINDOW_HEADER
#define GRAPHICS_WINDOW_HEADER

#include <vulkan/vulkan.h>
namespace Ui
{

struct WindowExtent
{
	int w;
	int h;
};

class IWindow
{
public:
	virtual ~IWindow() = default;

	virtual WindowExtent GetDimensions() const = 0;
	virtual void GetDimensions(int* w, int* h) const = 0;

	virtual VkSurfaceKHR CreateVulkanSurface(VkInstance instance) = 0;

	virtual uint32_t GetWindowID() = 0;

	// imgui functions
	virtual void InitImguiForVulkan() = 0;
	virtual void ShutdownImgui() = 0;

	virtual void BeginImGuiFrame() = 0;

	virtual void ProcessEvent(void* event) = 0;
};
}

#endif