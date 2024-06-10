#include "pch.hpp"
#include "Console.hpp"

// I'm not writing this
using namespace std::chrono_literals;
namespace stdc = std::chrono;

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
	if (!CanRun())
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
	if (!CanRun())
	{
		return false;
	}
	typedef stdc::steady_clock time;
	//TODO move this to application
	auto time_sice_last = stdc::duration_cast<stdc::microseconds>( time::now() - m_last_frame_start );

	while (time_sice_last < m_conf.FrameTime)
	{
		time_sice_last = stdc::duration_cast<stdc::microseconds>( time::now() - m_last_frame_start );
	}
	m_last_frame_start = time::now();

	do
	{
		Step();
	} while (!m_ppu.IsFrameDone() || m_masterClock % m_conf.PpuClockDivisor != 0);

	const auto end = time::now();
	const auto duration = stdc::duration_cast<stdc::microseconds>( end - m_last_frame_start );
	m_frame_time = duration.count() / 1e6;
	m_time_sice_last_frame = time_sice_last.count() / 1e6;
	return true;
}

bool Console::RunCpuInstruction()
{
	if (!m_cartridge)
	{
		return false;
	}
	do
	{
		Step();
	} while (!m_cpu.IsDone() || m_masterClock % m_conf.CpuClockDivisor != 0);
	return true;
}

bool Console::RunPpuPixel()
{
	if (!m_cartridge)
	{
		return false;
	}
	do
	{
		Step();
	} while (!m_ppu_done || m_ppu.GetCycles() > 256 && m_ppu.GetScanlines() > 240);
	return true;
}

bool Console::RunPpuScanline()
{
	if (!m_cartridge)
	{
		return false;
	}
	do
	{
		Step();
	} while (!m_ppu.IsScanlineDone() || m_masterClock % m_conf.PpuClockDivisor != 0);
	return true;
}

bool Console::RunPpuCycle()
{
	if (!m_cartridge)
	{
		return false;
	}
	do
	{
		Step();
	} while (!m_ppu_done);
	return true;
}

bool Console::RunCpuCycle()
{
	if (!m_cartridge)
	{
		return false;
	}
	do
	{
		Step();
	} while (!m_cpu_done);
	return true;
}

}