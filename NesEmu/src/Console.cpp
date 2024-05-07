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

Console::~Console()
{
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
	if (m_cartridge)
	{
		std::println("Resetting emulator");
		m_cpu.Reset();
		m_ppu.Reset();
		m_bus.Reset();
		m_cartridge = nullptr;
	}
}

void Console::LoadCartridge(const std::string& filepath)
{
	m_cartridge = std::make_unique<Cartridge>(filepath);
	m_cartridge->ConnectBus(&m_bus);
	m_bus.ConnectCartridge(m_cartridge.get());
	m_ppu.ConnectCartridge(m_cartridge.get());
}

bool Console::RunFrame()
{
	if (!m_cartridge)
	{
		return false;
	}
	while (!m_ppu.IsFrameDone())
	{
		Step();
	}
	return true;
}

}