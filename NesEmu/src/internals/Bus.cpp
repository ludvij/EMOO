#include "pch.hpp"
#include "Bus.hpp"

namespace Emu
{
Bus::Bus()
{
	m_cpu.ConnectBus(this);
}

u8 Bus::Read(u16 addr)
{
	// outside memory mirrors
	if (addr >= 0x2000) // commit sudoku
		throw std::exception("reading m_memory outside valid range");
	return m_memory[addr & 0x7ff];
}

void Bus::Write(u16 addr, u8 val)
{
	if (addr >= 0x2000) // commit sudoku
		throw std::exception("Writing m_memory outside valid range");
	m_memory[addr & 0x7ff] = val;
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
