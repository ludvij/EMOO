#include "pch.hpp"
#include "Bus.hpp"

namespace Emu
{

u8 Bus::Read(const u16 addr) const
{

	if (addr < 0x2000) // Ram and ram mirrors
	{
		return m_cpuRam[addr & 0x07FFF];
	}
	if (addr >= 0x2000 && addr < 0x4000) // PPU registers and mirrors
	{
		return m_ppu->CpuRead(addr);
	}
	if (addr >= 0x4000 && addr < 0x4018) // APU and IO functionality
	{
		if (addr == 0x4016 || addr == 0x4017) // JOY1 nad JOY2
		{
			return m_controller[addr & 0x0001]->Read();
		}
		if (addr <= 0x4013 || addr == 0x4015 || addr == 0x4017)
		{
			return m_apu->CpuRead(addr);
		}
		return 0;
	}
	if (addr >= 0x4018 && addr < 0x4020) // APU and IO functionality Test mode
	{
		return 0;
	}
	else // cartridge space
	{
		return *m_cartridge->CpuRead(addr);

	}

}

void Bus::Write(const u16 addr, const u8 val)
{
	if (addr < 0x2000) // Ram and ram mirrors
	{
		m_cpuRam[addr & 0x07FF] = val;
	}
	else if (addr >= 0x2000 && addr < 0x4000) // PPU registers and mirrors
	{
		m_ppu->CpuWrite(addr, val);
	}
	if (addr >= 0x4000 && addr < 0x4018) // APU and IO functionality
	{
		if (addr == 0x4016 || addr == 0x4017) // JOY1 nad JOY2
		{
			m_controller[addr & 0x0001]->Write();
		}
		if (addr <= 0x4013 || addr == 0x4015 || addr == 0x4017)
		{
			m_apu->CpuWrite(addr, val);
		}
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
