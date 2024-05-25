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
	m_bus.ConnectAPU(&m_apu);
	m_bus.ConnectController(0, &m_controller_ports[0]);
	m_bus.ConnectController(1, &m_controller_ports[1]);
	//m_apu.ConnectBus(&m_bus);
}

Console::~Console()
{
}

void Console::Step()
{
	m_cpu_done = false;
	m_ppu_done = false;
	if (m_masterClock % m_conf.CpuClockDivisor == 0)
	{
		if (m_ppu.IsDMATransfer())
		{
			m_bus.DMA(m_registered_cpu_cycles);
		}
		else
		{
			m_cpu.Step();
			m_cpu_done = true;
		}
		m_registered_cpu_cycles++;
	}
	if (m_masterClock % m_conf.PpuClockDivisor == 0)
	{
		m_ppu.Step();
		m_ppu_done = true;
		m_registered_ppu_cycles++;
	}

	if (m_ppu.IsNMI())
	{
		m_ppu.SetNMI(false);
		m_cpu.NMI();
	}
	m_masterClock++;
}

void Console::Reset()
{
	if (!m_cartridge)
	{
		return;
	}
	m_cpu.Reset();
	m_ppu.Reset();
	m_bus.Reset();
	m_apu.Reset();
	m_masterClock = 0;
	m_registered_cpu_cycles = 0;
	m_registered_ppu_cycles = 0;
}

void Console::LoadCartridge(const std::string& filepath)
{
	m_cartridge = std::make_shared<Cartridge>(filepath);
	m_cartridge->ConnectBus(&m_bus);
	m_bus.ConnectCartridge(m_cartridge);
	m_ppu.ConnectCartridge(m_cartridge);
	Reset();
}

void Console::LoadCartridgeFromMemory(const u8* data, const size_t size)
{
	m_cartridge = std::make_shared<Cartridge>(data, size);
	m_cartridge->ConnectBus(&m_bus);
	m_bus.ConnectCartridge(m_cartridge);
	m_ppu.ConnectCartridge(m_cartridge);
	Reset();
}

void Console::UnloadCartridge()
{
	Reset();
	m_cartridge = nullptr;
	m_bus.ConnectCartridge(nullptr);
	m_ppu.ConnectCartridge(nullptr);
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
	m_ppu.SetFrameDone(false);
	return true;
}

bool Console::RunCpuInstruction()
{
	if (!m_cartridge)
	{
		return false;
	}
	while (!m_cpu_done)
	{
		Step();
	}
	return true;
}

}