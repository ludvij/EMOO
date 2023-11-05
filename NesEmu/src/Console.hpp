#ifndef EMU_CONSOLE_HEADER
#define EMU_CONSOLE_HEADER

#include "internals/Core.hpp"
#include "internals/CPU.hpp"
#include "internals/Bus.hpp"
#include "internals/Cartridge.hpp"


#include <memory>

namespace Emu
{
	
class Console
{
public:
	Console();

	void Step();

	CPU& GetCpu()  { return m_cpu; }
	Bus& GetBus() { return m_bus; }

	void LoadCartridge(const std::string& filepath);

private:
	// Bus components
	CPU m_cpu;
	Bus m_bus;

	u32 m_masterClock = 0;

	std::shared_ptr<Cartridge> m_cartridge;
	
};

}

#endif