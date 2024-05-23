#ifndef EMU_IO_HEADER
#define EMU_IO_HEADER

#include "Core.hpp"

namespace Emu
{

enum Button : u8
{
	A      = 0x80,
	B      = 0x40,
	Select = 0x20,
	Start  = 0x10,
	Up     = 0x08,
	Down   = 0x04,
	Left   = 0x02,
	Right  = 0x01,
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