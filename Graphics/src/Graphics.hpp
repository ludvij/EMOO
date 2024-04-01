#ifndef EMU_GRAPHICS_HEADER
#define EMU_GRAPHICS_HEADER

#include <cstdint>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_vulkan.h>

namespace Graphics
{

struct Color
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
	uint8_t A = 0xff;
};

class Graphics
{
public:
	Graphics(uint32_t width, uint32_t height) : width(width), height(height) {};
	virtual ~Graphics() = 0;

	/**
	 * Initialises all info required for this to work
	 */
	virtual bool Init() = 0;

	/**
	 * Draws a pixel at given coordinates
	 */
	virtual bool DrawPixel(int32_t x, int32_t y, Color color) = 0;

	/**
	 * Renders all data sent
	 */
	virtual bool Present() = 0;

	private:

private:
	uint32_t width;
	uint32_t height;
};
} // namespace Graphics

#endif