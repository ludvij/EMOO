#include "pch.hpp"

#include "PPU.hpp"


namespace Emu
{
PPU::PPU(Configuration conf)
	: m_conf(conf)
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
	if (m_scanlines >= 0 && m_scanlines < 240 || m_scanlines == 261)
	{

		if (m_scanlines == 0 && m_cycles == 0)
		{
			m_cycles = 1;
		}
		if (m_cycles >= 2 && m_cycles < 256)
		{
			update_bg_shifters();
			preload_tile();
		}
		if (m_cycles == 256)
		{
			increment_y();
		}
		if (m_cycles == 257)
		{
			load_bg_shifters();
			reset_x();
		}
		// first two tiles on next scanline
		if (m_cycles >= 321 && m_cycles < 338)
		{
			update_bg_shifters();
			preload_tile();
		}
		// unused NT fetches
		if (m_cycles == 338 || m_cycles == 340)
		{
			m_bg_next_tile_id = memory_read(0x2000 | ( m_v & 0x0FFF ));
		}
		// pre render scanline
		if (m_scanlines == 261)
		{
			if (m_cycles == 1)
			{
				set_vBlank(false);
			}
			if (m_cycles >= 280 && m_cycles < 305)
			{
				reset_y();
			}
		}
	}
	if (m_scanlines == 240)
	{
		// post render scanline
	}
	// entering vBlank when scanline goes out of screen
	if (m_scanlines >= 240 && m_scanlines < 261)
	{
		if (m_scanlines == 241 && m_cycles == 1)
		{
			set_vBlank(true);
			// if nmi enabled
			if (m_ppu_ctrl & 0x80)
			{
				nmi = true;
			}
		}
	}



	if (( m_ppu_mask >> 3 ) & 0x01)
	{
		const u16 bit_mux = 0x8000 >> m_x;

		const u8 p0_pixel = ( m_bg_shifter_pattern_lo & bit_mux ) > 0;
		const u8 p1_pixel = ( m_bg_shifter_pattern_hi & bit_mux ) > 0;

		const u8 bg_pixel = ( p1_pixel << 1 ) | p0_pixel;

		const u8 bg_pal0 = ( m_bg_shifter_attrib_lo & bit_mux ) > 0;
		const u8 bg_pal1 = ( m_bg_shifter_attrib_hi & bit_mux ) > 0;

		const u8 bg_palette = ( bg_pal1 << 1 ) | bg_pal0;
		const int x = m_cycles - 1;
		const int y = m_scanlines;

		if (x >= 0 && x < m_conf.width && y >= 0 && y < m_conf.height)
		{
			const auto px_pos = static_cast<size_t>( x + y * 256 );
			m_screen[px_pos] = GetColorFromPalette(bg_palette, bg_pixel);

		}
	}

	m_cycles++;
	if (m_cycles >= 341)
	{
		m_cycles = 0;
		m_scanlines++;
		if (m_scanlines >= 262)
		{
			m_scanlines = 0;
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

	m_bg_next_tile_attrib = 0;
	m_bg_next_tile_hi = 0;
	m_bg_next_tile_hi = 0;
	m_bg_next_tile_id = 0;

	m_bg_shifter_attrib_hi = 0;
	m_bg_shifter_attrib_lo = 0;
	m_bg_shifter_pattern_hi = 0;
	m_bg_shifter_pattern_lo = 0;

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

		m_v += ( m_ppu_ctrl & 0x04 ) ? 32 : 1;
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
		m_t = ( ( static_cast<u16>( m_ppu_ctrl & 0x03 ) ) << 10 ) | ( m_t & ~0x0C00 );
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
		m_v += ( m_ppu_ctrl & 0x04 ) ? 32 : 1;
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
			return nametable_mirrored_read<MirrorName::A, MirrorName::B, MirrorName::A, MirrorName::B>(addr);
		}
		else if (m_cartridge->GetMirroring() == Cartridge::Mirroring::Horizontal)
		{
			return nametable_mirrored_read<MirrorName::A, MirrorName::A, MirrorName::B, MirrorName::B>(addr);
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
			nametable_mirrored_write<MirrorName::A, MirrorName::B, MirrorName::A, MirrorName::B>(addr, val);
		}
		else if (m_cartridge->GetMirroring() == Cartridge::Mirroring::Horizontal)
		{
			nametable_mirrored_write<MirrorName::A, MirrorName::A, MirrorName::B, MirrorName::B>(addr, val);
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

void PPU::preload_tile()
{
	switch (( m_cycles - 1 ) % 8)
	{
	case 0: // tile id
		load_bg_shifters();
		m_bg_next_tile_id = memory_read(0x2000 | ( m_v & 0x0FFF ));
		break;
	case 2:
		//        0000 0000 0000 0000
		// v    = _yyy NNYY YYYX XXXX
		// 0. 0x23C0                     offsets into range
		// d   -> 0010 0011 1100 0000 -> 0010 0011 1100 0000
		// 
		// 1. v & 0x0C00
		// v   -> _yyy NNYY YYYX XXXX    puts NN in bits 11 and 12
		// d   -> 0000 1100 0000 0000 -> 0000 NN00 0000 0000
		// 
		// 2. (v >> 4) & 0x38
		// v   -> _yyy NNYY YYYX XXXX
		// >>4 -> 0000 _yyy NNYY YYYX    puts hi Y in bits 3 4 5
		// d   -> 0000 0000 0011 1000 -> 0000 0000 00YY Y000
		// 
		// 3. (v >> 2) & 0x07
		// v   -> _yyy NNYY YYYX XXXX
		// >>2 -> 00_y yyNN YYYY YXXX    puts hi X in bits 0 1 2
		// d   -> 0000 0000 0000 0111 -> 0000 0000 0000 0XXX     -> 0010 NN11 11YY YXXX
		// result 0010 NN11 11YY YXXX
		// offset into vram, nametable select, attribute offset, hi 3 bits of coarse y, hi 3 bits of coarse x
		m_bg_next_tile_attrib = memory_read(0x23C0
			| ( m_v & 0x0C00 )
			| ( ( m_v >> 4 ) & 0x38 )
			| ( ( m_v >> 2 ) & 0x07 )
		);
		if (( m_v >> 5 ) & 0x0002) m_bg_next_tile_attrib >>= 4;
		if (( m_v >> 0 ) & 0x0002) m_bg_next_tile_attrib >>= 2;
		m_bg_next_tile_attrib &= 0x03;
		break;
	case 4:
		m_bg_next_tile_lo = memory_read(0
			// offset background nametable address
			+ ( m_ppu_ctrl & 0x10 ? 4096 : 0 )
			// add tile id * 16
			+ ( m_bg_next_tile_id * 16 )
			// add nametable position
			+ ( ( m_v >> 12 ) & 0x07 )
		);
		break;
	case 6:
		m_bg_next_tile_hi = memory_read(8
			+ ( m_ppu_ctrl & 0x10 ? 4096 : 0 )
			+ ( m_bg_next_tile_id * 16 )
			+ ( ( m_v >> 12 ) & 0x07 )
		);
		break;
	case 7:
		increment_x();
		break;
	}
}

void PPU::increment_y()
{
	if (!( ( m_ppu_mask >> 3 ) & 0x01 ) && !( ( m_ppu_mask >> 4 ) & 0x01 ))
	{
		return;
	}

	// check if fine y is lower than 7
	if (( m_v & 0x7000 ) != 0x7000)
	{
		// increment fine y
		m_v += 0x1000;
	}
	// fine y is greater than 7, we have to increment coarse y
	else
	{
		// reset fine y
		m_v &= ~0x7000;
		// extract coarse y
		int y = ( m_v & 0x03E0 ) >> 5;
		// check if coarse y is at the end of nametable
		if (y == 29)
		{
			y = 0;
			// flip nametable y bit
			m_v ^= 0x0800;
		}
		// in case we are in attribute memory
		else if (y == 31)
		{
			y = 0;
		}
		else
		{
			y += 1;
		}
		// replace coarse y
		m_v = ( m_v & ~0x03E0 ) | ( y << 5 );
	}
}

void PPU::increment_x()
{
	// if rendering backgournd or sprites
	if (!( ( m_ppu_mask >> 3 ) & 0x01 ) && !( ( m_ppu_mask >> 4 ) & 0x01 ))
	{
		return;
	}
	// nametables are 32x30 (0 index)
	// so if we are at the border
	// se have to set it to 0

	if (( m_v & 0x001F ) == 31)
	{
		// set coarse x to 0
		m_v &= ~0x001F;
		// flip nametable x
		m_v ^= 0x0400;
	}
	else
	{
		m_v += 1;
	}
}

void PPU::reset_x()
{
	// if rendering backgournd or sprites
	if (!( ( m_ppu_mask >> 3 ) & 0x01 ) && !( ( m_ppu_mask >> 4 ) & 0x01 ))
	{
		return;
	}
	// v = _yyy NNYY YYYX XXXX
	// swap nametable x and coarse x
	m_v = ( m_v & ~0x041F ) | ( m_t & 0x041F );
}

void PPU::reset_y()
{
	// if rendering backgournd or sprites
	if (!( ( m_ppu_mask >> 3 ) & 0x01 ) && !( ( m_ppu_mask >> 4 ) & 0x01 ))
	{
		return;
	}
	// v = _yyy NNYY YYYX XXXX
	// swap nametable y, coarse y and fine y
	m_v = ( m_v & ~0x7BE0 ) | ( m_t & 0x7BE0 );
}

void PPU::load_bg_shifters()
{
	m_bg_shifter_pattern_lo = ( m_bg_shifter_pattern_lo & 0xFF00 ) | m_bg_next_tile_lo;
	m_bg_shifter_pattern_hi = ( m_bg_shifter_pattern_hi & 0xFF00 ) | m_bg_next_tile_hi;

	m_bg_shifter_attrib_lo = ( m_bg_shifter_attrib_lo & 0xFF00 ) | ( ( m_bg_next_tile_attrib & 0x01 ) ? 0xFF : 0x00 );
	m_bg_shifter_attrib_hi = ( m_bg_shifter_attrib_hi & 0xFF00 ) | ( ( m_bg_next_tile_attrib & 0x02 ) ? 0xFF : 0x00 );
}

void PPU::update_bg_shifters()
{
	if (( m_ppu_mask >> 3 ) & 0x01)
	{
		m_bg_shifter_pattern_lo <<= 1;
		m_bg_shifter_pattern_hi <<= 1;

		m_bg_shifter_attrib_lo <<= 1;
		m_bg_shifter_attrib_hi <<= 1;
	}
}


Color PPU::GetColorFromPalette(const u8 palette, const u8 pixel)
{
	const u16 addr = 0x3F00 + ( palette * 4 ) + pixel;


	const u8 val = memory_read(addr) & 0x3F;

	return m_palette[val];

	//return m_palette[m_palette_ram_indexes[static_cast<size_t>( palette * 4 + pixel )]];
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
