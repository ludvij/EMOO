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

u8 PPU::memoryRead(const u16 addr)
{
	if (addr <= 0x0FFF) // pattern table 1
	{
		if (const auto val = m_cartridge->PpuRead(addr); val)
		{
			return *val;
		}
		else 
		{
			return m_patternTable[addr];
		}
	}	
	else if (0x1000 <= addr && addr <= 0x1FFF) // pattern table 2
	{
		if (const auto val = m_cartridge->PpuRead(addr); val)
		{
			return *val;
		}
		else 
		{
			return m_patternTable[addr];
		}
	}
	// https://www.nesdev.org/wiki/Mirroring#Nametable_Mirroring
	// $2000                - $2FFF
	//  0010 0000 0000 0000 -  0010 1111 1111 1111
	// $3000                - $3EFF
	//  0011 0000 0000 0000 -  0011 1110 1111 1111
	// nametable 0 $0000                - $03FF
	//              0000 0000 0000 0000 -  0000 0011 1111 1111
	// nametable 1 $0400                - $07FF
	//              0000 0100 0000 0000 -  0000 0111 1111 1111
	else if (0x2000 <= addr && addr <= 0x3EFF)
	{
		if (m_cartridge->GetMirroring() == Cartridge::Mirroring::Vertical)
		{
			return nametableMirroringRead<MirrorName::A, MirrorName::B, MirrorName::A, MirrorName::B>(addr);
		}
		else if (m_cartridge->GetMirroring() == Cartridge::Mirroring::Horizontal)
		{
			return nametableMirroringRead<MirrorName::A, MirrorName::A, MirrorName::B, MirrorName::B>(addr);
		}
	}
	Lud::Unreachable();
}

void PPU::memoryWrite(u16 addr, u8 val)
{
	
}



}
