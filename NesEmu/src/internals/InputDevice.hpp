#ifndef EMU_IO_HEADER
#define EMU_IO_HEADER

#include "Core.hpp"

namespace Emu
{

enum Button : u8
{
	A      = 0x80,
	B      = 0x40,
	SELECT = 0x20,
	START  = 0x10,
	UP     = 0x08,
	DOWN   = 0x04,
	LEFT   = 0x02,
	RIGHT  = 0x01,
};
class Controller
{
public:
	// for communication with used
	u8 data{};
public:
	void Write();
	u8 Read();
	u8 Peek() const;

	void SetPressed(Button button);
	void SetPressed(std::span<Button> buttons);

private:
	u8 m_status{};
};
}
#endif