#ifndef EMU_CONSOLE_HEADER
#define EMU_CONSOLE_HEADER

#include "internals/Core.hpp"

#include "internals/Bus.hpp"
#include "internals/CPU.hpp"
#include "internals/InputDevice.hpp"
#include "internals/PPU.hpp"


#include <chrono>
#include <memory>
#include <string_view>

namespace Emu
{

class Cartridge;

class Console : public Fman::ISerializable
{
public:
	Console(Configuration conf);
	~Console();

	void Step();



	void Reset();

	void LoadCartridge(const std::string& filepath);
	void LoadCartridgeFromMemory(std::string_view name, const u8* data, size_t size);

	void UnloadCartridge();

	bool RunFrame();

	bool RunCpuInstruction();

	bool RunPpuPixel();
	bool RunPpuScanline();
	bool RunPpuCycle();

	bool RunCpuCycle();

	void SaveState(int n);
	void LoadState(int n);

	bool CanRun() const
	{
		return m_cartridge != nullptr;
	}

	double GetFrameTime() const
	{
		return m_frame_time;
	}

	double GetTimeSinceLast() const
	{
		return m_time_since_last_frame;
	}

	CPU& GetCpu()
	{
		return m_cpu;
	}
	PPU& GetPpu()
	{
		return m_ppu;
	}
	Bus& GetBus()
	{
		return m_bus;
	}
	Configuration GetConfig() const
	{
		return m_conf;
	}

	Controller& GetController(u8 port)
	{
		return m_controller_ports[port];
	}

	// Inherited via ISerializable
	void Serialize(std::ostream& fs) override;

	void Deserialize(std::istream& fs) override;

private:
	// Bus components
	CPU m_cpu;
	PPU m_ppu;
	APU m_apu;
	Bus m_bus;

	double m_frame_time{ 0 };
	double m_time_since_last_frame{ 0 };

	u64 m_master_clock = 0;

	std::shared_ptr<Cartridge> m_cartridge;

	Controller m_controller_ports[2];

	// configuration struct containing platform related info (ntsc, pal)
	Configuration m_conf;

	u64 m_registered_cpu_cycles = 0;
	u64 m_registered_ppu_cycles = 0;

	bool m_cpu_done{ false };
	bool m_ppu_done{ false };

	std::chrono::time_point<std::chrono::steady_clock> m_last_frame_start;

};

}

#endif