#include "pch.hpp"

#include "PPU.hpp"


namespace Emu
{
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

u8 PPU::CpuRead(u16 addr)
{
	return 0;
}

void PPU::CpuWrite(const u16 addr, const u8 val)
{
	if (addr == 0x2006)
	{
		if (m_address_latch)
		{
			m_ppu_addr = val & 0xFF;
			m_address_latch = 1;
		}
		else
		{
			m_ppu_addr |= (val & 0x3F) << 8;
			m_address_latch = 1;
		}
	}
	else if (addr == 0x2007)
	{
		memoryWrite(m_ppu_addr, m_ppu_data);
		m_ppu_addr += m_ppu_ctrl >> 2 & 1;
	}
}

u8 PPU::memoryRead(const u16 addr)
{
	if (addr <= 0x0FFF) // pattern table 0
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
	else if (0x1000 <= addr && addr <= 0x1FFF) // pattern table 1
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
	else if (0x3F00 <= addr && addr <= 0x3FFF)
	{
		//https://www.nesdev.org/wiki/PPU_palettes#Memory_Map
		u16 strippedAddr = addr & 0x001F;
		if      (addr == 0x0010) strippedAddr = 0x0000;
		else if (addr == 0x0014) strippedAddr = 0x0004;
		else if (addr == 0x0018) strippedAddr = 0x0008;
		else if (addr == 0x001C) strippedAddr = 0x000C;
		return m_palleteRamIndexes[addr];
	}
	Lud::Unreachable();
}

void PPU::memoryWrite(u16 addr, u8 val)
{
	if (addr <= 0x0FFF) // pattern table 0
	{
		if (!m_cartridge->PpuWrite(addr, val))
		{
			m_patternTable[addr] = val;
		}
	}	
	else if (0x1000 <= addr && addr <= 0x1FFF) // pattern table 1
	{
		if (!m_cartridge->PpuWrite(addr, val))
		{
			m_patternTable[addr] = val;
		}
	}
	// https://www.nesdev.org/wiki/Mirroring#Nametable_Mirroring
	else if (0x2000 <= addr && addr <= 0x3EFF)
	{
		if (m_cartridge->GetMirroring() == Cartridge::Mirroring::Vertical)
		{
			nametableMirroringWrite<MirrorName::A, MirrorName::B, MirrorName::A, MirrorName::B>(addr, val);
		}
		else if (m_cartridge->GetMirroring() == Cartridge::Mirroring::Horizontal)
		{
			nametableMirroringWrite<MirrorName::A, MirrorName::A, MirrorName::B, MirrorName::B>(addr, val);
		}
	}
	else if (0x3F00 <= addr && addr <= 0x3FFF)
	{
		//https://www.nesdev.org/wiki/PPU_palettes#Memory_Map
		u16 strippedAddr = addr & 0x001F;
		if      (addr == 0x0010) strippedAddr = 0x0000;
		else if (addr == 0x0014) strippedAddr = 0x0004;
		else if (addr == 0x0018) strippedAddr = 0x0008;
		else if (addr == 0x001C) strippedAddr = 0x000C;
		m_palleteRamIndexes[addr] = val;
	}
	Lud::Unreachable();
}



}
