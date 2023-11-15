#include "pch.hpp"
#include "Bus.hpp"

namespace Emu
{
// wanted to see how the javadoc thing fares
/**
 * @brief Translates the address provided into an address minus an offset mirrored to size exclussive
 * @param addr The address to be translated
 * @param size frequency of mirrors, size is exclussive, operation is addr % (size - 1)
 * @param offset default is 0, translates address given the offest by substracting offset to address
 * 
 */
constexpr auto TranslateMirroredAddress = [](const u16 addr, const u16 size, const u16 offset=0) -> u16
{ 
	return (addr - offset) % (size - 1); 
};

u8 Bus::Read(const u16 addr) const
{
	if (addr < 0x2000) // Ram and ram mirrors
	{
		return m_cpuRam[TranslateMirroredAddress(addr, 0x800)];
	}
	if (addr >= 0x2000 && addr < 0x4000) // PPU registers and mirrors
	{
		return m_ppuRegisters[TranslateMirroredAddress(addr, 0x8, 0x2000)];
	}
	if (addr >= 0x4000 && addr < 0x4018) // APU and IO functionality
	{
	}
	if (addr >= 0x4018 && addr < 0x4020) // APU and IO functionality Test mode
	{
	}
	else // cartridge space
	{
		return m_cartridge->CpuRead(addr);
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
		m_ppuRegisters[TranslateMirroredAddress(addr, 0x8, 0x2000)] = val;
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

}
