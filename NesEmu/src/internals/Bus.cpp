#include "pch.hpp"
#include "Bus.hpp"

namespace Emu
{
Bus::Bus()
{
	m_cpu.ConnectBus(this);
}

u8 Bus::Read(u16 addr) const 
{
	if (addr < 0x2000) // Ram and ram mirrors
	{
		return m_mem[addr & 0x07ff];
	}
	if (addr < 0x4000) // PPU registers and mirrors
	{
	}
	if (addr < 0x4018) // APU and IO functionality
	{
	}
	if (addr < 0x4020) // APU and IO functionality Test mode
	{
	}
	// cartridge space
	return m_mem[addr];
}

void Bus::Write(u16 addr, u8 val)
{
	if (addr < 0x2000) // Ram and ram mirrors
	{
		m_mem[addr & 0x07ff] = val;
	}
	// else if (addr < 0x4000) // PPU registers and mirrors
	// {
	// }
	// else if (addr < 0x4018) // APU and IO functionality
	// {
	// }
	// else if (addr < 0x4020) // APU and IO functionality Test mode
	// {
	// }
	else // cartridge space
	{
		m_mem[addr] = val;
	}

	
}

void Bus::Step()
{
	if (m_masterClock % NTSC_CLOCK_DIVISOR == 0) 
	{
		m_cpu.Step();
	}
	m_masterClock++;
}
}
