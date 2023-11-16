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
	if (0 <= addr && addr <= 0x0FFF) // pattern table 1
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
	else if (0x2000 <= addr && addr <= 0x23FF) // nametable 0
	{
		return m_ram[addr - 0x2000];
	}
	else if (0x2400 <= addr && addr <= 0x27FF) // nametable 1
	{
		return m_ram[addr - 0x2000];
	}
	else if (0x2800 <= addr && addr <= 0x2BFF) // nametable 2
	{
		if (const auto val = m_cartridge->PpuRead(addr); val)
		{
			return *val;
		}
		else 
		{
			return m_ram[addr - 0x2800];
		}
	}
	else if (0x2C00 <= addr && addr <= 0x2FFF) // nametable 3
	{
		if (const auto val = m_cartridge->PpuRead(addr); val)
		{
			return *val;
		}
		else 
		{
			return m_ram[addr - 0x2800];
		}
	}
	else if (0x3000 <= addr && addr <= 0x3EFF)
	{
		// nametalbe mirroring, I have to check this
	}
}

void PPU::memoryWrite(u16 addr, u8 val)
{
	
}

}
