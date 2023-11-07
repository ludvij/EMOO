#ifndef EMU_BUS_HEADER
#define EMU_BUS_HEADER

#include "Core.hpp"

#include <array>
#include <memory>

namespace Emu
{
class Cartridge;
class Bus
{
public:

	void Write(u16 addr, u8 val);
	u8 Read(u16 addr) const;

	void ConnectCartridge(const std::shared_ptr<Cartridge> cartridge) { m_cartridge = cartridge; }

private:
	// maybe do this in a single array in the form of
	// 0x4020 to reach everything until the cartridge
	// the problem is that this approach wastes a ton of memory
	// 0x3020 bytes to be exact
	// std::array<u8, 0x4020> m_memor{0};
	std::array<u8, 0x800> m_cpuRam{0};
	std::array<u8, 0x8  > m_ppuRam{0};
	std::shared_ptr<Cartridge> m_cartridge;
};

}

#endif