#ifndef EMU_CONSOLE_HEADER
#define EMU_CONSOLE_HEADER

#include "internals/Core.hpp"

#include "internals/Bus.hpp"
#include "internals/CPU.hpp"
#include "internals/InputDevice.hpp"
#include "internals/PPU.hpp"

#include <memory>

namespace Emu
{

class Cartridge;

class Console
{
public:
	Console(Configuration conf);
	~Console();

	void Step();


	void Reset();

	void LoadCartridge(const std::string& filepath);
	void LoadCartridgeFromMemory(const u8* data, size_t size);

	void UnloadCartridge();

	bool RunFrame();

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

	u32* OutputScreen()
	{
		return m_ppu.GetScreen();
	};

	bool HasUpdatedPatternTables()
	{
		return m_ppu.HasUpdatedPatternTables();
	}

	bool HasUpdatedPalettes()
	{
		return m_ppu.HasUpdatedPalettes();
	}

	u32* OutputPatternTable(u8 palette)
	{
		return m_ppu.GetPatternTable(palette);
	}

	u32 OutputPaletteColor(u8 i, u8 s)
	{
		return m_ppu.GetColorFromPalette(i, s);
	}

	u32* OutputPalette()
	{
		return m_ppu.GetPalette();
	}

	Controller& GetController(u8 port)
	{
		return m_controller_ports[port];
	}

private:
	// Bus components
	CPU m_cpu;
	PPU m_ppu;
	APU m_apu;
	Bus m_bus;

	u64 m_masterClock = 0;

	std::shared_ptr<Cartridge> m_cartridge;

	Controller m_controller_ports[2];

	// configuration struct containing platform related info (ntsc, pal)
	Configuration m_conf;

	u64 m_registered_cpu_cycles = 0;
	u64 m_registered_ppu_cycles = 0;

};

}

#endif