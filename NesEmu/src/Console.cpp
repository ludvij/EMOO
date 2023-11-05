#include "pch.hpp"
#include "Console.hpp"

namespace Emu
{
Console::Console()
{
	m_cpu.ConnectBus(&m_bus);
	m_cartridge->ConnectBus(&m_bus);
}

void Console::Step()
{
	if (m_masterClock % NTSC_CLOCK_DIVISOR == 0) 
	{
		m_cpu.Step();
	}
	m_masterClock++;
}

void Console::LoadCartridge(const std::string& filepath)
{
	m_cartridge = std::make_shared<Cartridge>(filepath);
}
}
