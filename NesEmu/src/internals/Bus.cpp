#include "pch.hpp"
#include "Bus.hpp"

namespace Emu
{

u8 Bus::Read(const u16 addr) const
{
	if (addr < 0x2000) // Ram and ram mirrors
	{
		return m_cpuRam[addr & 0x07FF];
	}
	if (addr < 0x4000) // PPU registers and mirrors
	{
		return m_ppuRam[(addr - 0x2000) & 0x07FF];
	}
	if (addr < 0x4018) // APU and IO functionality
	{
	}
	if (addr < 0x4020) // APU and IO functionality Test mode
	{
	}
	else // cartridge space
	{
		return m_cartridge->CpuRead(addr - 0x4020);
	}
}


void Bus::Write(const u16 addr, const u8 val)
{
	if (addr < 0x2000) // Ram and ram mirrors
	{
		m_cpuRam[addr & 0x07FF] = val;
	}
	else if (addr < 0x4000) // PPU registers and mirrors
	{
		m_ppuRam[(addr - 0x2000) & 0x07FF] = val;
	}
	else if (addr < 0x4018) // APU and IO functionality
	{
	}
	else if (addr < 0x4020) // APU and IO functionality Test mode
	{
	}
	else if(addr <= 0xFFFF) // cartridge space
	{
		m_cartridge->CpuWrite(addr - 0x4020, val);
	}
}

}
