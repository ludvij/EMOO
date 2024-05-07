#include "pch.hpp"
#include "Bus.hpp"

namespace Emu
{

static constexpr auto TranslateMirroredAddress(const u16 addr, const u16 size, const u16 offset=0) -> u16
{
	return ( addr - offset ) % ( size - 1 );
};

u8 Bus::Read(const u16 addr) const
{
	if (addr < 0x2000) // Ram and ram mirrors
	{
		return m_cpuRam[TranslateMirroredAddress(addr, 0x800)];
	}
	if (addr >= 0x2000 && addr < 0x4000) // PPU registers and mirrors
	{
		return m_ppu->CpuRead(addr);
	}
	//if (addr >= 0x4000 && addr < 0x4018) // APU and IO functionality
	//{
	//}
	//if (addr >= 0x4018 && addr < 0x4020) // APU and IO functionality Test mode
	//{
	//}
	else // cartridge space
	{
		return *m_cartridge->CpuRead(addr);
	}

}

void Bus::Write(const u16 addr, const u8 val)
{
	if (addr < 0x2000) // Ram and ram mirrors
	{
		m_cpuRam[TranslateMirroredAddress(addr, 0x800)] = val;
	}
	else if (addr >= 0x2000 && addr < 0x4000) // PPU registers and mirrors
	{
		m_ppu->CpuWrite(addr, val);
	}
	if (addr >= 0x4000 && addr < 0x4018) // APU and IO functionality
	{
	}
	if (addr >= 0x4018 && addr < 0x4020) // APU and IO functionality Test mode
	{
	}
	else // cartridge space
	{
		m_cartridge->CpuWrite(addr, val);
	}
}

void Bus::Reset()
{
}

}
