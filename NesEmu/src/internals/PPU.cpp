#include "pch.hpp"

#include "PPU.hpp"


namespace Emu
{
PPU::PPU(Configuration conf)
{
	load_palette(conf.palette_src);
	const size_t size = static_cast<size_t>( conf.width * conf.height );
	m_screen = new u32[size];
	std::fill_n(m_screen, size, 0xFF000000);
}
PPU::~PPU()
{
	delete[] m_screen;
}
void PPU::Step()
{
	m_cycles++;
	// a single frame takes 241 * 262 cycles 
	// basically this plots a pixel per cycle since the nes screen is 256 * 240
	// 20 of those scanlines is tanek in vblank


	if (m_cycles >= 257 && m_cycles <= 320)
	{
		m_oam_addr = 0;
	}
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

void PPU::Reset()
{
	m_ppu_ctrl = 0;
	m_ppu_mask = 0;
	m_scanlines = 0;
	m_cycles = 0;
	m_x = 0;
	m_v = 0;
	m_t = 0;
	m_w = 0;
	m_data_buffer = 0;
	m_ppu_status = 0;
}

u8 PPU::CpuRead(const u16 addr)
{
	u8 data;
	if (addr == PPU_STATUS_ADDR) // status
	{
		m_w = 0;
		data = m_ppu_status;
		m_ppu_status ^= ( 1 << 7 );
	}
	// https://www.nesdev.org/wiki/PPU_registers#Data_($2007)_%3C%3E_read/write
	else if (addr == OAM_DATA_ADDR) // oam data
	{
		data = m_oam_data[addr];
	}
	else if (addr == PPU_DATA_ADDR)
	{
		// reads before the palette range [0, 0x3f00] are delayed one cycle
		data = m_data_buffer;
		m_data_buffer = memory_read(m_v);
		// reads in the palette range return inmediately 
		if (m_v >= 0x3F00) data = m_data_buffer;

		m_v += ( m_ppu_ctrl & 0b0000'0010 ) ? 32 : 1;
	}
	else  // cheat so the compiler shuts up
	{
		Lud::Unreachable();
	}
	return data;
}

void PPU::CpuWrite(const u16 addr, const u8 val)
{
	if (addr == PPU_CTRL_ADDR) // control
	{
		m_ppu_ctrl = val;
		// set nametables
		m_t = ( ( m_ppu_ctrl & 0b0000'0011 ) << 10 ) | ( m_t & 0b1100'1111 );
	}
	else if (addr == PPU_MASK_ADDR) // mask
	{
		m_ppu_mask = val;
	}
	else if (addr == OAM_ADDR_ADDR) // oam address
	{
		m_oam_addr = val;
	}
	else if (addr == OAM_DATA_ADDR)
	{
		if (m_scanlines > 239)
		{
			m_oam_data[m_oam_addr] = val;
			m_oam_addr++;

		}
	}
	else if (addr == PPU_SCROLL_ADDR) // ppu scroll
	{
		write_ppu_scroll(val);
	}
	else if (addr == PPU_ADDR_ADDR)
	{
		write_ppu_addr(val);
	}
	else if (addr == PPU_DATA_ADDR)
	{
		memory_write(m_v, m_ppu_data);
		m_v += ( m_ppu_ctrl & 0b0000'0010 ) ? 32 : 1;
	}
}

u32* PPU::GetScreen() const
{
	return m_screen;
}

u8 PPU::memory_read(const u16 addr)
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
		if (addr == 0x0010) strippedAddr = 0x0000;
		else if (addr == 0x0014) strippedAddr = 0x0004;
		else if (addr == 0x0018) strippedAddr = 0x0008;
		else if (addr == 0x001C) strippedAddr = 0x000C;
		return m_palleteRamIndexes[strippedAddr];
	}
	Lud::Unreachable();
}

void PPU::memory_write(u16 addr, u8 val)
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
		if (addr == 0x0010) strippedAddr = 0x0000;
		else if (addr == 0x0014) strippedAddr = 0x0004;
		else if (addr == 0x0018) strippedAddr = 0x0008;
		else if (addr == 0x001C) strippedAddr = 0x000C;
		m_palleteRamIndexes[strippedAddr] = val;
	}
	Lud::Unreachable();
}

void PPU::write_ppu_addr(const u8 val)
{
	if (!m_w)
	{
		// clear hi byte
		m_t &= 0x00FF;
		// mirror down hi byte
		m_t = static_cast<u16>( ( val & 0x3F ) << 8 ) | ( m_t & 0x00FF );
		m_w = 1;
	}
	else
	{
		// clear lo byte

		m_t = static_cast<u16>( m_t & 0xFF00 ) | val;
		m_v = m_t;
		m_w = 0;
	}
}

// during rendering registers t and v can be seen as
// yyy NN YYYYY XXXXX
// ||| || ||||| └┴┴┴┴> corse x scroll
// ||| || └┴┴┴┴──────> coarse y scroll
// ||| └┴────────────> nametable select
// └┴┴───────────────> fine y scroll
//                     fine x scroll is stored in x register
void PPU::write_ppu_scroll(const u8 val)
{
	if (!m_w)
	{
		// store 3 lo bits of val into fine x
		m_x = val & 0x7;
		// store 5 hi bits of val into coarse x of t (bits from 0 to 4)
		m_t = ( val >> 3 ) | ( m_t & 0b111'11'11111'00000 );
		m_w = 1;
	}
	else
	{
		// store 3 lo bits of val into fine y (bits from 12 to 14)
		m_t = ( ( val & 0x7 ) << 12 ) | ( m_t & 0b000'11'11111'11111 );
		// store 5 hi bits of val into coarse y (bits from 5 to 9)
		m_t = ( ( val >> 3 ) << 5 ) | ( m_t & 0b111'11'00000'11111 );
		m_w = 0;
	}

}

void PPU::load_palette(const char* src)
{
	std::ifstream file(src);
	// TODO: add more sophisticated handling
#ifdef NES_EMU_DEBUG
	if (!file.is_open())
	{
		std::throw_with_nested(std::runtime_error("File not found"));
	}
#endif // NES_EMU_DEBUG
	file.read(std::bit_cast<char*>( m_palette.data() ), sizeof m_palette);
}



}
