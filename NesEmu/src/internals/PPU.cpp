#include "pch.hpp"

#include "PPU.hpp"


namespace Emu
{
PPU::PPU(Configuration conf)
{
	load_palette(conf.palette_src);
	const size_t size = static_cast<size_t>( conf.width * conf.height );
	m_screen.resize(size);
	std::ranges::fill(m_screen, Color(0));


}
PPU::~PPU()
{
}
void PPU::Step()
{
	if (m_scanlines == -1 && m_cycles == 1)
	{
		set_vBlank(false);
	}
	// entering vBlank when scanline goes out of screen
	if (m_scanlines == 241 && m_cycles == 1)
	{
		set_vBlank(true);
		// if nmi enabled
		if (m_ppu_ctrl & 0x80)
		{
			nmi = true;
		}
	}
	m_cycles++;
	if (m_cycles >= 341)
	{
		m_cycles = 0;
		m_scanlines++;
		if (m_scanlines >= 261)
		{
			m_scanlines = -1;
			isFrameDone = true;
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
	std::fill(m_palette_ram_indexes.begin(), m_palette_ram_indexes.end(), 0);

}

u8 PPU::CpuRead(const u16 addr)
{
	u8 data;
	if (addr == PPU_STATUS_ADDR) // status
	{
		// takes 3 hi bits from status and the next is stale ppu bus contents
		data = ( m_ppu_status & 0xE0 ) | ( m_data_buffer & 0x1F );
		// Reading the status register will clear bit 7 and also the address latch used by PPUSCROLL and PPUADDR.
		set_vBlank(false);
		m_w = 0;
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
		if (m_v > 0x3F00)
		{
			data = m_data_buffer;
		}

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
		memory_write(m_v, val);
		m_v += ( m_ppu_ctrl & 0b0000'0010 ) ? 32 : 1;
	}
}

u32* PPU::GetScreen()
{
	return m_screen.data();
}

u8 PPU::memory_read(u16 addr)
{
	addr &= 0x3FFF;
	if (addr <= 0x0FFF) // pattern table 0
	{
		if (!m_cartridge)
		{
			return m_pattern_tables[addr];
		}
		else if (const auto val = m_cartridge->PpuRead(addr); val)
		{
			return *val;
		}
		else
		{
			return m_pattern_tables[addr];
		}
	}
	else if (0x1000 <= addr && addr <= 0x1FFF) // pattern table 1
	{
		if (!m_cartridge)
		{
			return m_pattern_tables[addr];
		}
		else if (const auto val = m_cartridge->PpuRead(addr); val)
		{
			return *val;
		}
		else
		{
			return m_pattern_tables[addr];
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
		if (strippedAddr == 0x0010) strippedAddr = 0x0000;
		if (strippedAddr == 0x0014) strippedAddr = 0x0004;
		if (strippedAddr == 0x0018) strippedAddr = 0x0008;
		if (strippedAddr == 0x001C) strippedAddr = 0x000C;
		return m_palette_ram_indexes[strippedAddr];
	}
	Lud::Unreachable();
}

void PPU::memory_write(u16 addr, const u8 val)
{
	addr &= 0x3FFF;
	if (addr <= 0x0FFF) // pattern table 0
	{
		if (m_cartridge && !m_cartridge->PpuWrite(addr, val))
		{
			m_pattern_tables[addr] = val;
		}
	}
	else if (0x1000 <= addr && addr <= 0x1FFF) // pattern table 1
	{
		if (m_cartridge && !m_cartridge->PpuWrite(addr, val))
		{
			m_pattern_tables[addr] = val;
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
		if (strippedAddr == 0x0010) strippedAddr = 0x0000;
		if (strippedAddr == 0x0014) strippedAddr = 0x0004;
		if (strippedAddr == 0x0018) strippedAddr = 0x0008;
		if (strippedAddr == 0x001C) strippedAddr = 0x000C;
		m_palette_ram_indexes[strippedAddr] = val;
	}
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

Color PPU::GetColorFromPalette(const u8 palette, const u8 pixel)
{
	const u16 addr = 0x3F00 + ( palette * 4 ) + pixel;

	const u8 val = memory_read(addr) & 0x3F;

	return m_palette[val];
}

u32* PPU::GetPatternTable(const u8 i, const u8 palette)
{
	for (u16 y = 0; y < 16; y++)
	{
		for (u16 x = 0; x < 16; x++)
		{
			// y * 256 + x * 16 == y * 16 * 16 + x * 16
			//                     y * 16 * 16 + x * 16 == (y * 16 + x) * 16
			u16 offset = y * 256 + x * 16;

			for (u16 row = 0; row < 8; row++)
			{
				u8 tile_lo = memory_read(i * 0x1000 + offset + row + 0);
				u8 tile_hi = memory_read(i * 0x1000 + offset + row + 8);

				for (u16 col = 0; col < 8; col++)
				{
					// can be either 00,01,10,11
					// 00 is background/transparent
					// else is color index
					u8 pixel = ( tile_lo & 0x01 ) + ( tile_hi & 0x01 );
					tile_hi >>= 1;
					tile_lo >>= 1;

					const u16 pattern_x = x * 8 + ( 7 - col );
					const u16 pattern_y = y * 8 + row;
					m_pattern_tables_show[i][static_cast<size_t>( pattern_x + pattern_y * 128 )] = GetColorFromPalette(palette, pixel);
				}
			}
		}
	}
	return m_pattern_tables_show[i].data();
}

u32* PPU::GetPalette()
{
	for (size_t i = 0; i < m_palette.size(); i++)
	{
		m_palettes_show[i] = m_palette[i];
	}
	return m_palettes_show.data();
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

void PPU::set_vBlank(bool set)
{
	if (set)
	{
		m_ppu_status |= 0x80;
	}
	else
	{
		m_ppu_status &= ~0x80;
	}
}



Color::Color(u8 val)
	: Color(val, val, val)
{

}

Color::Color(u8 R, u8 B, u8 G)
	: R(R)
	, G(G)
	, B(B)
{
}

u32 Color::AsU32() const
{
	if constexpr (std::endian::native == std::endian::little)
	{
		return 0xFF << 24 | B << 16 | G << 8 | R;
	}
	else
	{
		return R << 24 | G << 16 | B << 8 | 0xFF;
	}
}

}
