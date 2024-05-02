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
	virtual ~IWindow()
	{
	};

	virtual WindowExtent GetDimensions() const = 0;
	virtual void GetDimensions(int* w, int* h) const = 0;

	virtual VkSurfaceKHR CreateVulkanSurface(VkInstance instance) = 0;

	virtual uint32_t GetWindowID() = 0;

	virtual void InitImguiForVulkan() = 0;
};
}

#endif