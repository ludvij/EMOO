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
	m_bus.ConnectAPU(&m_apu); // placeholder for eventual APU implementation
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
	if (m_master_clock % m_conf.cpu_clock_divisor == 0)
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
	if (m_master_clock % m_conf.ppu_clock_divisor == 0)
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
	m_master_clock++;
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
	m_master_clock = 0;
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

void Console::LoadCartridgeFromMemory(std::string_view name, const u8* data, const size_t size)
{
	m_cartridge = std::make_shared<Cartridge>(name, data, size);
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

	while (time_sice_last < m_conf.frame_time)
	{
		time_sice_last = stdc::duration_cast<stdc::microseconds>( time::now() - m_last_frame_start );
	}
	m_last_frame_start = time::now();

	do
	{
		Step();
	} while (!m_ppu.IsFrameDone() || m_master_clock % m_conf.ppu_clock_divisor != 0);

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
	} while (!m_cpu.IsDone() || m_master_clock % m_conf.cpu_clock_divisor != 0);
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
	} while (!m_ppu_done || m_ppu.GetCycles() > 256 || m_ppu.GetScanlines() > 240);
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
	} while (!m_ppu.IsScanlineDone() || m_master_clock % m_conf.ppu_clock_divisor != 0);
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

void Console::SaveState(int n)
{
	if (!CanRun())
	{
		return;
	}
	Fman::PushFolder("state");
	{
		Fman::PushFolder(m_cartridge->GetROMName());
		{
			Fman::SetSerializeFilename(std::format("state_{:d}", n));
			Fman::Serialize(this);
		}
	}
	Fman::PopFolder(2);
}

// does nothing if states does not exist
void Console::LoadState(int n)
{
	if (!CanRun())
	{
		return;
	}
	Fman::PushFolder("state");
	{
		Fman::PushFolder(m_cartridge->GetROMName());
		{
			Fman::SetSerializeFilename(std::format("state_{:d}", n));
			Fman::Deserialize(this);
		}
	}
	Fman::PopFolder(2);
}

void Console::Serialize(std::fstream& fs)
{
	Fman::SerializeStatic(m_frame_time);
	Fman::SerializeStatic(m_time_sice_last_frame);
	Fman::SerializeStatic(m_master_clock);
	Fman::SerializeStatic(m_registered_cpu_cycles);
	Fman::SerializeStatic(m_registered_ppu_cycles);
	Fman::SerializeStatic(m_cpu_done);
	Fman::SerializeStatic(m_ppu_done);
	m_cpu.Serialize(fs);
	m_bus.Serialize(fs);
	m_ppu.Serialize(fs);
	m_cartridge->Serialize(fs);
	m_ppu.Serialize(fs);
	m_controller_ports[0].Serialize(fs);
	m_controller_ports[1].Serialize(fs);
}

void Console::Deserialize(std::fstream& fs)
{
	Fman::DeserializeStatic(m_frame_time);
	Fman::DeserializeStatic(m_time_sice_last_frame);
	Fman::DeserializeStatic(m_master_clock);
	Fman::DeserializeStatic(m_registered_cpu_cycles);
	Fman::DeserializeStatic(m_registered_ppu_cycles);
	Fman::DeserializeStatic(m_cpu_done);
	Fman::DeserializeStatic(m_ppu_done);
	m_cpu.Deserialize(fs);
	m_bus.Deserialize(fs);
	m_ppu.Deserialize(fs);
	m_cartridge->Deserialize(fs);
	m_controller_ports[0].Deserialize(fs);
	m_controller_ports[1].Deserialize(fs);
}

}