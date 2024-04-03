#include "pch.hpp"
#include "Console.hpp"

namespace Emu
{
Console::Console(Configuration conf = NTSC)
	: m_conf(conf)
	, m_ppu(conf)
{
	m_cpu.ConnectBus(&m_bus);
	m_bus.ConnectPPU(&m_ppu);
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

void Console::Reset()
{
#ifdef NES_EMU_DEBUG
	std::cout << "Resetting emulator\n";
#endif // NES_EMU_DEBUG
	if (m_cartridge) 
	{
		m_cpu.Reset();
		m_ppu.Reset();
	}
}

void Console::LoadCartridge(const std::string& filepath)
{
	m_cartridge = std::make_shared<Cartridge>(filepath);
	m_cartridge->ConnectBus(&m_bus);
	m_bus.ConnectCartridge(m_cartridge);
	m_ppu.ConnectCartridge(m_cartridge);
}

}