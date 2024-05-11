#include "pch.hpp"
#include "InputDevice.hpp"

namespace Emu
{
// replace status with data provided by user
void Controller::Write()
{
	m_status = data;
}
u8 Controller::Read()
{
	data = 0;
	u8 res = ( m_status & 0x80 ) > 0;
	m_status <<= 1;
	return res;
}

void Controller::SetPressed(Button button)
{
	data |= 1 << std::to_underlying(button);
}

void Controller::SetPressed(std::span<Button> buttons)
{
	for (const auto& button : buttons)
	{
		data |= 1 << std::to_underlying(button);
	}
}
}