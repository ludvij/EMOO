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

	u32* OutputPatternTable(u8 i, u8 palette)
	{
		return m_ppu.GetPatternTable(i, palette);
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

	void SetCloseOperation(std::function<void()> fn)
	{
		m_cpu.SetCloseCallbackApplication(fn);
	}

private:
	// Bus components
	CPU m_cpu;
	PPU m_ppu;
	Bus m_bus;

	u32 m_masterClock = 0;

	std::shared_ptr<Cartridge> m_cartridge;

	Controller m_controller_ports[2];

	// configuration struct containing platform related info (ntsc, pal)
	Configuration m_conf;

	bool m_can_run = true;
};

}

#endif