#include "InputDevice.hpp"
#include "pch.hpp"

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

u8 Controller::Peek() const
{
	return m_status;
}

void Controller::SetPressed(Button button)
{
	data |= button;
}

void Controller::SetPressed(std::span<Button> buttons)
{
	for (const auto& button : buttons)
	{
		data |= button;
	}
}
void Controller::Serialize(std::ostream& fs)
{
	Fman::SerializeStatic(fs, data);
	Fman::SerializeStatic(fs, m_status);
}
void Controller::Deserialize(std::istream& fs)
{
	Fman::DeserializeStatic(fs, data);
	Fman::DeserializeStatic(fs, m_status);
}
}