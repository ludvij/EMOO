#ifndef EMU_CONSOLE_HEADER
#define EMU_CONSOLE_HEADER

#include "internals/Core.hpp"

#include "internals/Bus.hpp"
#include "internals/CPU.hpp"
#include "internals/PPU.hpp"

#include <memory>

namespace Emu
{

class Cartridge;

class Console
{
public:
	Console(Configuration conf);

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

	u32* OutputScreen() const
	{
		return m_ppu.GetScreen();
	};

private:
	// Bus components
	CPU m_cpu;
	PPU m_ppu;
	Bus m_bus;

	u32 m_masterClock = 0;

	std::shared_ptr<Cartridge> m_cartridge;

	// configuration struct containing platform related info (ntsc, pal)
	Configuration m_conf;
};

}

#endif