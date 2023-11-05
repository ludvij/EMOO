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
	std::array<u8, 0x10000> m_cpuRam{0};
	std::array<u8, 0x800> m_ppuRam{0};
	std::shared_ptr<Cartridge> m_cartridge;
};

}

#endif