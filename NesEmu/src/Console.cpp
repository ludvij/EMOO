#include "pch.hpp"
#include "Console.hpp"

namespace Emu
{
Console::Console(Configuration conf)
	: m_conf(conf)
{
	m_cpu.ConnectBus(&m_bus);
	m_ppu.ConnectBus(&m_bus);
	//m_apu.ConnectBus(&m_bus);
}

void Console::Step()
{
	if (m_masterClock % m_conf.CpuClockDivisor == 0) 
	{
		m_cpu.Step();
	}
	if (m_masterClock % m_conf.PpuClockDivisor == 0)
	{
		m_ppu.Step();
	}
	m_masterClock++;
}

void Console::LoadCartridge(const std::string& filepath)
{
	m_cartridge = std::make_shared<Cartridge>(filepath);
	m_cartridge->ConnectBus(&m_bus);
	m_bus.ConnectCartridge(m_cartridge);
}

}