#ifndef EMU_IO_HEADER
#define EMU_IO_HEADER

#include "Core.hpp"

namespace Emu
{

enum class Button : u8
{
	A      = 7,
	B      = 6,
	Select = 5,
	Start  = 4,
	Up     = 3,
	Down   = 2,
	Left   = 1,
	Right  = 0,
};
class Controller
{
public:
	// for communication with used
	u8 data{};
public:
	void Write();
	u8 Read();

	void SetPressed(Button button);
	void SetPressed(std::span<Button> buttons);

private:
	u8 m_status{};
};
}
#endif