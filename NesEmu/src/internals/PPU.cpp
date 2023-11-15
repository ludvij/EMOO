#include "pch.hpp"

#include "PPU.hpp"

#include "Bus.hpp"

namespace Emu
{
void PPU::ConnectBus(Bus *bus)
{
	m_bus = bus;
	PPUCTRL.Link(bus);
	PPUMASK.Link(bus);
	PPUSCROLL.Link(bus);
	OAMADDR.Link(bus);
	OAMDATA.Link(bus);
	PPUSCROLL.Link(bus);
	PPUADDR.Link(bus);
	PPUDATA.Link(bus);
	OAMDMA.Link(bus);
}

void PPU::Step()
{
	m_cycles++;
	// a single frame takes 241 * 262 cycles 
	// basically this plots a pixel per cycle since the nes screen is 256 * 240
	// 20 of those scanlines is tanek in vblank
	if (m_cycles >= 341)
	{
		m_cycles = 0;
		m_scanlines++;
		if (m_scanlines >= 262)
		{
			m_scanlines = -1;
		}
	}
}

u8 PPU::memoryRead(u16 addr)
{
	
}

void PPU::memoryWrite(u16 addr, u8 val)
{
	
}

}
