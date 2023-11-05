#include "pch.hpp"
#include "Bus.hpp"

namespace Emu
{

u8 Bus::Read(u16 addr) const
{
	if (addr < 0x2000) // Ram and ram mirrors
	{
		return m_cpuRam[addr & 0x07FF];
	}
	if (addr < 0x4000) // PPU registers and mirrors
	{
		return m_ppuRam[addr & 0x07ff + 0x2000];
	}
	if (addr < 0x4018) // APU and IO functionality
	{
	}
	if (addr < 0x4020) // APU and IO functionality Test mode
	{
	}
	// cartridge space
	return m_cpuRam[addr];
}


void Bus::Write(u16 addr, u8 val)
{
	if (addr < 0x2000) // Ram and ram mirrors
	{
		m_cpuRam[addr & 0x07FF] = val;
	}
	else if (addr < 0x4000) // PPU registers and mirrors
	{
		m_ppuRam[addr & 0x07FF + 0x2000] = val;
	}
	// else if (addr < 0x4018) // APU and IO functionality
	// {
	// }
	// else if (addr < 0x4020) // APU and IO functionality Test mode
	// {
	// }
	else // cartridge space
	{
		m_cpuRam[addr] = val;
	}
}

}
