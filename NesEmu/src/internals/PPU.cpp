#include "pch.hpp"

#include "palettes/Default.hpp"
#include "PPU.hpp"

namespace Emu
{
PPU::PPU(Configuration conf)
	: m_conf(conf)
{
	load_palette();
	const size_t size = static_cast<size_t>( conf.width * conf.height );
	m_screen = new u32[256 * 240];
	m_pattern_tables_show = new u32[128 * 128 * 2];
	std::fill_n(m_screen, 256 * 240, Color(0, 0, 0));
	std::fill_n(m_pattern_tables_show, 128 * 128 * 2, Color(0, 0, 0));
	//std::memset(m_OAM, 0, 64 * sizeof ObjectAttributeEntry);


}
PPU::~PPU()
{
	delete[] m_screen;
	delete[] m_pattern_tables_show;
}
void PPU::Step()
{
	m_scanline_done = false;
	m_cycle++;
	// visible scanlines
	if (m_scanline >= 0 && m_scanline < 240)
	{
		if (m_cycle >= 1 && m_cycle < 257)
		{
			update_bg_shifters();
			preload_tile();
			spirte_evaluation();

			draw_pixel();
			if (m_cycle == 256)
			{
				increment_y();
			}
		}
		else if (m_cycle >= 257 && m_cycle < 321)
		{
			m_oam_addr = 0;
			if (m_cycle == 257)
			{
				load_bg_shifters();
				reset_x();
			}
		}
		else if (m_cycle >= 321 && m_cycle < 338)
		{
			update_bg_shifters();
			preload_tile();
		}
		// first two tiles on next scanline
		// unused NT fetches
		else if (m_cycle == 338 || m_cycle == 340)
		{
			m_bg_next_tile_id = memory_read(0x2000 | ( m_v & 0x0FFF ));
		}
	}
	// post render scanline, idle
	else if (m_scanline == 240)
	{
	}
	// entering vBlank when scanline goes out of screen
	else if (m_scanline == 241 && m_cycle == 1)
	{
		m_ppu_status.set_flags(Status::Flags::VERTICAL_BLANK, true);
		if (m_ppu_ctrl.is_flag_set(Control::Flags::GENERATE_NMI))
		{
			m_nmi = true;
		}
	}
	else if (m_scanline == 261)
	{
		if (m_cycle == 1)
		{
			m_ppu_status.set_flags(Status::Flags::VERTICAL_BLANK, false);
			m_ppu_status.set_flags(Status::Flags::SPRITE_0_HIT, false);
		}
		if (m_cycle >= 1 && m_cycle < 257)
		{
			update_bg_shifters();
			preload_tile();
			if (m_cycle == 256)
			{
				increment_y();
			}
		}
		else if (m_cycle >= 257 && m_cycle < 321)
		{
			m_oam_addr = 0;
			if (m_cycle == 257)
			{
				load_bg_shifters();
				reset_x();
			}
			else if (m_cycle >= 280 && m_cycle < 305)
			{
				reset_y();
			}
		}
		else if (m_cycle >= 321 && m_cycle < 338)
		{
			update_bg_shifters();
			preload_tile();
		}
		//For odd frames, the cycle at the end of the scanline is skipped (this is done internally by jumping directly from (339,261) to (0,0)
		if (m_cycle == 339 && m_frames & 0x01)
		{
			m_cycle++;
		}
	}

	if (m_cycle >= 340)
	{
		m_cycle = 0;
		m_scanline++;
		m_scanline_done = true;
		if (m_scanline >= 262)
		{
			m_scanline = 0;
			m_frame_done = true;
			m_frames++;
		}
	}
}

bool PPU::IsFrameDone() const
{
	return m_frame_done;
}

void PPU::SetFrameDone(bool set)
{
	m_frame_done = set;
}

bool PPU::IsNMI() const
{
	return m_nmi;
}

void PPU::SetNMI(bool set)
{
	m_nmi = set;
}

bool PPU::IsDMATransfer() const
{
	return m_dma_transfer;
}

bool PPU::IsDMADummy() const
{
	return m_dma_dummy;
}

void PPU::SetDMADummy(bool set)
{
	m_dma_dummy = set;
}

void PPU::SetDMAData(u8 val)
{
	m_dma_data = val;
}

u16 PPU::GetDMAAddr() const
{
	return static_cast<u16>( ( m_dma_page << 8 ) | m_dma_addr );
}

ObjectAttributeEntry PPU::GetOAMEntry(size_t entry) const
{
	return m_OAM[entry];
}


bool PPU::HasUpdatedPatternTables()
{
	const bool copy = m_updated_patterns;
	m_updated_patterns = false;
	return copy;
}

bool PPU::HasUpdatedPalettes()
{
	const bool copy = m_updated_palettes;
	m_updated_palettes = false;
	return copy;
}

void PPU::Reset()
{
	m_ppu_ctrl = 0;
	m_ppu_mask = 0;
	m_scanline = 0;
	m_cycle = 0;
	m_x = 0;
	m_v = 0;
	m_t = 0;
	m_w = 0;
	m_data_buffer = 0;
	m_ppu_status = 0;

	std::ranges::fill(m_palette_ram_indexes, Color(0, 0, 0));
	std::fill_n(m_screen, 256 * 240, Color(0, 0, 0));

	m_bg_next_tile_attrib = 0;
	m_bg_next_tile_hi = 0;
	m_bg_next_tile_hi = 0;
	m_bg_next_tile_id = 0;

	m_bg_shifter_attrib_hi = 0;
	m_bg_shifter_attrib_lo = 0;
	m_bg_shifter_pattern_hi = 0;
	m_bg_shifter_pattern_lo = 0;

	m_dma_addr = 0;
	m_dma_data = 0;
	m_dma_page = 0;
	m_dma_dummy = true;
	m_dma_transfer = false;
	m_nmi = false;

	m_oam_addr = 0;
	m_oam_copy_buffer = 0;
	m_oam_keep_copying = false;
	m_oam_sprite_bounded = false;
	m_oam_n = 0;
	m_oam_m = 0;
	m_oam_dma = 0;
	m_secondary_oam_addr = 0;
	m_secondary_oam_val = 0;
	m_current_sprite_count = 0;

	std::ranges::fill(m_secondary_OAM, 0);
	std::memset(m_OAM, 0, 64);


}

u8 PPU::CpuRead(const u16 addr)
{
	u8 data;
	if (addr == PPU_STATUS_ADDR) // status
	{
		// takes 3 hi bits from status and the next is stale ppu bus contents
		data = ( m_ppu_status & Status::Flags::ALL ) | ( m_data_buffer & 0x1F );
		// Reading the status register will clear bit 7 and also the address latch used by PPUSCROLL and PPUADDR.
		m_ppu_status.set_flags(Status::Flags::VERTICAL_BLANK, false);
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

		m_v += get_vram_increment_mode();
	}
	else  // cheat so the compiler shuts up
	{
		Lud::Unreachable();
	}
	return data;
}

u8 PPU::CpuPeek(u16 addr) const
{
	u8 data = 0;
	if (addr == PPU_STATUS_ADDR) // status
	{
		// takes 3 hi bits from status and the next is stale ppu bus contents
		data = ( m_ppu_status & Status::Flags::ALL ) | ( m_data_buffer & 0x1F );
		// Reading the status register will clear bit 7 and also the address latch used by PPUSCROLL and PPUADDR.
	}
	// https://www.nesdev.org/wiki/PPU_registers#Data_($2007)_%3C%3E_read/write
	else if (addr == OAM_DATA_ADDR) // oam data
	{
		data = m_oam_data[addr];
	}
	else if (addr == PPU_DATA_ADDR)
	{
		data = memory_read(m_v);
	}
	return data;
}

void PPU::CpuWrite(const u16 addr, const u8 val)
{
	if (addr == PPU_CTRL_ADDR) // control
	{
		m_ppu_ctrl = val;
		// set nametables
		m_t = ( ( m_ppu_ctrl & Control::Flags::NAMETABLE_BASE_ADDR ) << 10 ) | ( m_t & ~0x0C00 );
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
		if (m_scanline > 239)
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
		m_v += get_vram_increment_mode();
	}
	else if (addr == OAM_DMA_ADDR)
	{
		m_dma_transfer = true;
		m_dma_page = val;
		m_dma_addr = 0x00;
	}
}

void PPU::DMA()
{
	m_oam_data[m_dma_addr] = m_dma_data;
	// unsigned integer overflow is defined behaviour
	m_dma_addr++;
	if (m_dma_addr == 0x00)
	{
		m_dma_dummy = true;
		m_dma_transfer = false;
	}
}

u32* PPU::GetScreen()
{
	return m_screen;
}

u8 PPU::memory_read(u16 addr) const
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
	else if (addr <= 0x1FFF) // pattern table 1
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
	else if (addr <= 0x3EFF)
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
	else if (addr <= 0x3FFF)
	{
		return read_palette_ram_indexes(addr);
	}
	Lud::Unreachable();
}

void PPU::memory_write(u16 addr, const u8 val)
{
	addr &= 0x3FFF;
	if (addr <= 0x0FFF) // pattern table 0
	{
		m_updated_patterns = true;
		if (m_cartridge && !m_cartridge->PpuWrite(addr, val))
		{
			m_pattern_tables[addr] = val;
		}
	}
	else if (addr <= 0x1FFF) // pattern table 1
	{
		m_updated_patterns = true;
		if (m_cartridge && !m_cartridge->PpuWrite(addr, val))
		{
			m_pattern_tables[addr] = val;
		}
	}
	// https://www.nesdev.org/wiki/Mirroring#Nametable_Mirroring
	else if (addr <= 0x3EFF)
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
	else if (addr <= 0x3FFF)
	{
		m_updated_palettes = true;
		write_palette_ram_indexes(addr, val);
	}
}

void PPU::write_ppu_addr(const u8 val)
{
	if (!m_w)
	{
		// clear hi byte
		m_t &= 0x00FF;
		// mirror down hi byte
		m_t = ( ( val & 0x3F ) << 8 ) | ( m_t & 0x00FF );
		m_w = 1;
	}
	else
	{
		// clear lo byte

		m_t = ( m_t & 0xFF00 ) | val;
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
		m_t = ( val >> 3 ) | ( m_t & ~0x001F );
		m_w = 1;
	}
	else
	{
		// store 3 lo bits of val into fine y (bits from 12 to 14)
		m_t = ( ( val & 0x7 ) << 12 ) | ( m_t & ~0x7000 );
		// store 5 hi bits of val into coarse y (bits from 5 to 9)
		m_t = ( ( val >> 3 ) << 5 ) | ( m_t & ~0x03E0 );
		m_w = 0;
	}

}


void PPU::spirte_evaluation()
{
	// secondary OAM clear
	if (m_cycle < 65)
	{
		// it's literally faster to do this every frame instead of cheking for even frames
		m_oam_copy_buffer = 0xFF;
		m_secondary_OAM[static_cast<size_t>( ( m_cycle - 1 ) >> 1 )] = m_oam_copy_buffer;
	}
	// sprite evaluation for next scanline
	else if (m_cycle < 257)
	{
		if (m_cycle == 65)
		{
			// clear flag thingy
			m_added_sprite_0 = false;
			m_oam_keep_copying = true;
			m_oam_sprite_bounded = false;

			m_secondary_oam_addr = 0;
			m_current_sprite_count = 0;

			m_oam_n = ( m_oam_addr >> 2 ) & 0x3F;
			m_oam_m = m_oam_addr & 0x03;
		}
		// entering HBlank
		else if (m_cycle == 256)
		{
			m_current_sprite_count = m_secondary_oam_addr >> 2;
		}
		if (m_cycle % 2 == 1)
		{
			m_oam_copy_buffer = m_oam_data[m_oam_addr];
		}
		else
		{
			if (m_oam_keep_copying)
			{

				// let s = m_scanline
				// let b = m_oam_copy_buffer
				// y = s - b
				// to be in range:
				// 1. y >= 0       => (s - b) >= 0       => s >= b
				// 2. y <  16 or 8 => (s - b) <  16 or 8 => s <  b + 16 or 8
				const int y = m_scanline - m_oam_copy_buffer;
				if (!m_oam_sprite_bounded && y >= 0 && y < get_sprite_y_size())
				{
					m_oam_sprite_bounded = true;
				}
				if (m_secondary_oam_addr < 32)
				{
					m_secondary_OAM[m_secondary_oam_addr] = m_oam_copy_buffer;
					// copy sprite
					if (m_oam_sprite_bounded)
					{
						m_oam_m++;
						m_secondary_oam_addr++;

						// checking sprite 0 hits
						if (m_oam_n == 0)
						{
							m_added_sprite_0 = true;
						}

						// done copying sprite
						if (( m_secondary_oam_addr & 0x03 ) == 0)
						{
							m_oam_sprite_bounded = false;
							m_oam_m = 0;
							m_oam_n = ( m_oam_n + 1 ) & 0x3F;
							if (m_oam_n == 0)
							{
								m_oam_keep_copying = false;
							}
						}
					}
					// not copy sprite
					else
					{
						m_oam_n = ( m_oam_n + 1 ) & 0x3F;
						if (m_oam_n == 0)
						{
							m_oam_keep_copying = false;
						}
					}
				}
				// more than 8 sprites
				else
				{
					m_oam_copy_buffer = m_secondary_OAM[m_secondary_oam_addr & 0x1F];

					if (m_oam_sprite_bounded)
					{
						m_ppu_status.set_flags(Status::Flags::SPRITE_OVERFLOW, true);
						m_oam_m++;
						if (m_oam_m == 4)
						{
							m_oam_m = 0;
							m_oam_n = ( m_oam_n + 1 ) & 0x3F;
						}
					}
					// sprite overflow bug
					else
					{
						m_oam_n = ( m_oam_n + 1 ) & 0x3F;
						m_oam_m = ( m_oam_m + 1 ) & 0x03;
						if (m_oam_n == 0)
						{
							m_oam_keep_copying = false;
						}
					}
				}
			}
			else
			{
				m_oam_n = ( m_oam_n + 1 ) & 0x3F;
				// when more than 8 sprites have been found
				// we read from secondary oam
				if (m_secondary_oam_addr >= 32)
				{
					m_oam_copy_buffer = m_secondary_OAM[m_secondary_oam_addr & 0x1F];
				}

			}
			m_oam_addr = m_oam_n * 4 + m_oam_m;
		}

	}
}

bool PPU::is_rendering_enabled()
{
	return m_ppu_mask.is_flag_set(Mask::Flags::SHOW_BACKGROUND | Mask::Flags::SHOW_SPRITES);
}

void PPU::preload_tile()
{
	switch (( m_cycle - 1 ) % 8)
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
		m_bg_next_tile_addr = 0
			// offset background nametable address
			+ ( ( m_ppu_ctrl & Control::Flags::BACKGROUND_PATTERN_ADDR ) << 8 )
			// add tile id * 16
			+ ( m_bg_next_tile_id * 16 )
			// add nametable position
			+ ( ( m_v >> 12 ) & 0x07 );
		m_bg_next_tile_lo = memory_read(m_bg_next_tile_addr);
		break;
	case 6:
		m_bg_next_tile_hi = memory_read(m_bg_next_tile_addr + 8);
		break;
	case 7:
		increment_x();
		break;
	}
}

void PPU::increment_y()
{
	if (!can_show_background_or_sprites())
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
			y++;
		}
		// replace coarse y
		m_v = ( m_v & ~0x03E0 ) | ( y << 5 );
	}
}

void PPU::increment_x()
{
	// if rendering backgournd or sprites
	if (!can_show_background_or_sprites())
	{
		return;
	}
	// nametables are 32x30 (0 index)
	// so if we are at the border
	// se have to set it to 0

	if (( m_v & 0x001F ) == 31)
	{
		// set coarse x to 0
		// flip nametable x
		m_v = ( m_v & ~0x001F ) ^ 0x0400;
	}
	else
	{
		m_v++;;
	}
}

void PPU::reset_x()
{
	// if rendering backgournd or sprites
	if (!can_show_background_or_sprites())
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
	if (!can_show_background_or_sprites())
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
	if (can_show_background())
	{
		m_bg_shifter_pattern_lo <<= 1;
		m_bg_shifter_pattern_hi <<= 1;

		m_bg_shifter_attrib_lo <<= 1;
		m_bg_shifter_attrib_hi <<= 1;
	}
}

u8 PPU::get_vram_increment_mode() const
{
	return m_ppu_ctrl.is_flag_set(Control::Flags::VRAM_INCREMENT) ? 32 : 1;
}

u8 PPU::get_sprite_y_size() const
{
	return m_ppu_ctrl.is_flag_set(Control::Flags::SPRITE_SIZE_SELECT) ? 16 : 8;
}


bool PPU::can_show_background() const
{
	return m_ppu_mask.is_flag_set(Mask::Flags::SHOW_BACKGROUND);
}

bool PPU::can_show_sprites() const
{
	return m_ppu_mask.is_flag_set(Mask::Flags::SHOW_SPRITES);
}
// 0 0 -> 0 ~ 1
// 1 0 -> 1 ~ 0
// 0 1 -> 1 ~ 0
// 1 1 -> 1 ~ 0
bool PPU::can_show_background_or_sprites() const
{
	return m_ppu_mask.is_flag_set(Mask::Flags::SHOW_SPRITES | Mask::Flags::SHOW_BACKGROUND);
}

u32 PPU::read_palette_ram_indexes(u16 addr) const
{
	addr &= 0x1F;
	if (addr == 0x10 || addr == 0x14 || addr == 0x18 || addr == 0x1C)
	{
		addr &= ~0x10;
	}
	return m_palette_ram_indexes[addr];
}

void PPU::write_palette_ram_indexes(u16 addr, u8 val)
{
	addr &= 0x1F;
	val &= 0x3F;
	if (addr == 0x00 || addr == 0x10)
	{
		m_palette_ram_indexes[0x00] = val;
		m_palette_ram_indexes[0x10] = val;
	}
	else if (addr == 0x04 || addr == 0x14)
	{
		m_palette_ram_indexes[0x04] = val;
		m_palette_ram_indexes[0x14] = val;
	}
	else if (addr == 0x08 || addr == 0x18)
	{
		m_palette_ram_indexes[0x08] = val;
		m_palette_ram_indexes[0x18] = val;
	}
	else if (addr == 0x0C || addr == 0x1C)
	{
		m_palette_ram_indexes[0x0C] = val;
		m_palette_ram_indexes[0x1C] = val;
	}
	else
	{
		m_palette_ram_indexes[addr] = val;
	}
}

u32 PPU::get_color_for_pixel()
{
	u32 color = 0;
	if (can_show_background())
	{
		const u16 bit_mux = 0x8000 >> m_x;

		const u8 p0_pixel = ( m_bg_shifter_pattern_lo & bit_mux ) > 0;
		const u8 p1_pixel = ( m_bg_shifter_pattern_hi & bit_mux ) > 0;

		const u8 bg_pixel = ( p1_pixel << 1 ) | p0_pixel;

		const u8 bg_pal0 = ( m_bg_shifter_attrib_lo & bit_mux ) > 0;
		const u8 bg_pal1 = ( m_bg_shifter_attrib_hi & bit_mux ) > 0;

		const u8 bg_palette = ( bg_pal1 << 1 ) | bg_pal0;


		color = GetColorFromPalette(bg_palette, bg_pixel);
	}

	return color;
}

void PPU::draw_pixel()
{
	if (can_show_background_or_sprites() || ( m_v & 0x3F00 ) != 0x3F00)
	{
		m_screen[( m_scanline << 8 ) + m_cycle - 1] = get_color_for_pixel();
	}
	else
	{
		m_screen[( m_scanline << 8 ) + m_cycle - 1] = m_palette_ram_indexes[m_v & 0x1F];
	}
}


Color PPU::GetColorFromPalette(const u8 palette, const u8 pixel)
{
	return m_palette[m_palette_ram_indexes[static_cast<size_t>( palette * 4 + pixel )]];
}


u32* PPU::GetPatternTable(const u8 idx, const u8 palette)
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
				u8 tile_lo = memory_read(idx * 0x1000 + offset + row + 0);
				u8 tile_hi = memory_read(idx * 0x1000 + offset + row + 8);

				for (u16 col = 0; col < 8; col++)
				{
					// can be either 00,01,10,11
					// 00 is background/transparent
					// else is color index
					u8 pixel = ( ( tile_lo & 0x01 ) << 1 ) | ( tile_hi & 0x01 );
					tile_hi >>= 1;
					tile_lo >>= 1;

					const u16 pattern_x = x * 8 + ( 7 - col );
					const u16 pattern_y = y * 8 + row;
					const size_t pos = static_cast<size_t>( idx * 128 * 128 + pattern_x + pattern_y * 128 );
					m_pattern_tables_show[pos] = GetColorFromPalette(palette, pixel);
				}
			}
		}
	}

	return m_pattern_tables_show + idx * 128 * 128;
}

u32* PPU::GetPalette()
{
	std::memcpy(m_palettes_show.data(), m_palette.data(), sizeof m_palette);

	return m_palettes_show.data();
}

void PPU::load_palette()
{
	std::memcpy(m_palette.data(), palettes_NTSC_pal, palettes_NTSC_pal_len);
}


Color::Color(u8 val) : Color(val, val, val)
{

}

Color::Color(u8 R, u8 G, u8 B) : R(R), G(G), B(B)
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

bool RegisterFlags::is_flag_set(u8 flags) const
{
	return reg & flags;
}

void RegisterFlags::set_flags(u8 flags, bool set)
{
	if (set)
	{
		reg |= flags;
	}
	else
	{
		reg &= ~flags;
	}
}

u8 RegisterFlags::operator|(u8 flags) const
{
	return reg & flags;
}

}
