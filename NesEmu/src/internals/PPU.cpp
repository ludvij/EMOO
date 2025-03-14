#include "pch.hpp"

#include "palettes/Default.hpp"
#include "PPU.hpp"

static constexpr u8 BitReverseTable256[256] =
{
	0b0000'0000, 0b1000'0000, 0b0100'0000, 0b1100'0000, 0b0010'0000, 0b1010'0000, 0b0110'0000, 0b1110'0000,
	0b0001'0000, 0b1001'0000, 0b0101'0000, 0b1101'0000, 0b0011'0000, 0b1011'0000, 0b0111'0000, 0b1111'0000,
	0b0000'1000, 0b1000'1000, 0b0100'1000, 0b1100'1000, 0b0010'1000, 0b1010'1000, 0b0110'1000, 0b1110'1000,
	0b0001'1000, 0b1001'1000, 0b0101'1000, 0b1101'1000, 0b0011'1000, 0b1011'1000, 0b0111'1000, 0b1111'1000,
	0b0000'0100, 0b1000'0100, 0b0100'0100, 0b1100'0100, 0b0010'0100, 0b1010'0100, 0b0110'0100, 0b1110'0100,
	0b0001'0100, 0b1001'0100, 0b0101'0100, 0b1101'0100, 0b0011'0100, 0b1011'0100, 0b0111'0100, 0b1111'0100,
	0b0000'1100, 0b1000'1100, 0b0100'1100, 0b1100'1100, 0b0010'1100, 0b1010'1100, 0b0110'1100, 0b1110'1100,
	0b0001'1100, 0b1001'1100, 0b0101'1100, 0b1101'1100, 0b0011'1100, 0b1011'1100, 0b0111'1100, 0b1111'1100,
	0b0000'0010, 0b1000'0010, 0b0100'0010, 0b1100'0010, 0b0010'0010, 0b1010'0010, 0b0110'0010, 0b1110'0010,
	0b0001'0010, 0b1001'0010, 0b0101'0010, 0b1101'0010, 0b0011'0010, 0b1011'0010, 0b0111'0010, 0b1111'0010,
	0b0000'1010, 0b1000'1010, 0b0100'1010, 0b1100'1010, 0b0010'1010, 0b1010'1010, 0b0110'1010, 0b1110'1010,
	0b0001'1010, 0b1001'1010, 0b0101'1010, 0b1101'1010, 0b0011'1010, 0b1011'1010, 0b0111'1010, 0b1111'1010,
	0b0000'0110, 0b1000'0110, 0b0100'0110, 0b1100'0110, 0b0010'0110, 0b1010'0110, 0b0110'0110, 0b1110'0110,
	0b0001'0110, 0b1001'0110, 0b0101'0110, 0b1101'0110, 0b0011'0110, 0b1011'0110, 0b0111'0110, 0b1111'0110,
	0b0000'1110, 0b1000'1110, 0b0100'1110, 0b1100'1110, 0b0010'1110, 0b1010'1110, 0b0110'1110, 0b1110'1110,
	0b0001'1110, 0b1001'1110, 0b0101'1110, 0b1101'1110, 0b0011'1110, 0b1011'1110, 0b0111'1110, 0b1111'1110,
	0b0000'0001, 0b1000'0001, 0b0100'0001, 0b1100'0001, 0b0010'0001, 0b1010'0001, 0b0110'0001, 0b1110'0001,
	0b0001'0001, 0b1001'0001, 0b0101'0001, 0b1101'0001, 0b0011'0001, 0b1011'0001, 0b0111'0001, 0b1111'0001,
	0b0000'1001, 0b1000'1001, 0b0100'1001, 0b1100'1001, 0b0010'1001, 0b1010'1001, 0b0110'1001, 0b1110'1001,
	0b0001'1001, 0b1001'1001, 0b0101'1001, 0b1101'1001, 0b0011'1001, 0b1011'1001, 0b0111'1001, 0b1111'1001,
	0b0000'0101, 0b1000'0101, 0b0100'0101, 0b1100'0101, 0b0010'0101, 0b1010'0101, 0b0110'0101, 0b1110'0101,
	0b0001'0101, 0b1001'0101, 0b0101'0101, 0b1101'0101, 0b0011'0101, 0b1011'0101, 0b0111'0101, 0b1111'0101,
	0b0000'1101, 0b1000'1101, 0b0100'1101, 0b1100'1101, 0b0010'1101, 0b1010'1101, 0b0110'1101, 0b1110'1101,
	0b0001'1101, 0b1001'1101, 0b0101'1101, 0b1101'1101, 0b0011'1101, 0b1011'1101, 0b0111'1101, 0b1111'1101,
	0b0000'0011, 0b1000'0011, 0b0100'0011, 0b1100'0011, 0b0010'0011, 0b1010'0011, 0b0110'0011, 0b1110'0011,
	0b0001'0011, 0b1001'0011, 0b0101'0011, 0b1101'0011, 0b0011'0011, 0b1011'0011, 0b0111'0011, 0b1111'0011,
	0b0000'1011, 0b1000'1011, 0b0100'1011, 0b1100'1011, 0b0010'1011, 0b1010'1011, 0b0110'1011, 0b1110'1011,
	0b0001'1011, 0b1001'1011, 0b0101'1011, 0b1101'1011, 0b0011'1011, 0b1011'1011, 0b0111'1011, 0b1111'1011,
	0b0000'0111, 0b1000'0111, 0b0100'0111, 0b1100'0111, 0b0010'0111, 0b1010'0111, 0b0110'0111, 0b1110'0111,
	0b0001'0111, 0b1001'0111, 0b0101'0111, 0b1101'0111, 0b0011'0111, 0b1011'0111, 0b0111'0111, 0b1111'0111,
	0b0000'1111, 0b1000'1111, 0b0100'1111, 0b1100'1111, 0b0010'1111, 0b1010'1111, 0b0110'1111, 0b1110'1111,
	0b0001'1111, 0b1001'1111, 0b0101'1111, 0b1101'1111, 0b0011'1111, 0b1011'1111, 0b0111'1111, 0b1111'1111,
};

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
	m_frame_done = false;
	m_cycle++;
	// visible scanlines
	if (m_scanline >= 0 && m_scanline < 240)
	{
		if (m_cycle >= 1 && m_cycle < 257)
		{
			update_shifters();
			preload_tile();
			sprite_evaluation();

			draw_pixel();
			if (m_cycle == 256)
			{
				increment_y();
			}
		}
		else if (m_cycle >= 257 && m_cycle < 321)
		{
			if (is_rendering_enabled())
			{
				m_oam_addr = 0;
			}
			if (m_cycle == 257)
			{
				load_bg_shifters();
				reset_x();
			}
			load_sprite_shifters();
		}
		else if (m_cycle >= 321 && m_cycle < 338)
		{
			update_shifters();
			preload_tile();
		}
		// garbage NT
		else if (m_cycle == 338 || m_cycle == 340)
		{
			memory_read(get_nametable_addr());
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
			m_ppu_status.set_flags(Status::Flags::SPRITE_OVERFLOW, false);

			for (size_t i = 0; i < 8; i++)
			{
				m_sprite_shifter_pattern_hi[i] = 0;
				m_sprite_shifter_pattern_lo[i] = 0;
			}
		}
		if (m_cycle >= 1 && m_cycle < 257)
		{
			update_shifters();
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
			update_shifters();
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
	m_frames = 0;
	m_x = 0;
	m_v = 0;
	m_t = 0;
	m_w = 0;
	m_data_buffer = 0;
	m_ppu_status = 0;

	std::ranges::fill(m_palette_ram_indexes, Color(0, 0, 0));
	std::fill_n(m_screen, 256 * 240, Color(0, 0, 0));
	std::ranges::fill(m_scanline_sprites, SpriteInfo{ false, false, 0, 0 });
	m_current_sprite_count = 0;

	m_bg_next_tile_attrib = 0;
	m_bg_next_tile_hi = 0;
	m_bg_next_tile_hi = 0;
	m_bg_next_tile_id = 0;

	m_bg_shifter_attrib_hi = 0;
	m_bg_shifter_attrib_lo = 0;
	m_bg_shifter_pattern_hi = 0;
	m_bg_shifter_pattern_lo = 0;

	std::ranges::fill(m_sprite_shifter_pattern_lo, 0);
	std::ranges::fill(m_sprite_shifter_pattern_hi, 0);
	m_current_sprite_addr = 0;

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
	m_current_oam_pos = 0;
	m_secondary_oam_addr = 0;
	m_secondary_oam_val = 0;
	m_current_sprite_count = 0;

	std::ranges::fill(m_secondary_OAM, 0);
	std::fill_n(m_oam_data, 64 * 4, 0);


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
	if (addr == PPU_CTRL_ADDR)
	{
		data = m_ppu_ctrl;
	}
	else if (addr == PPU_MASK_ADDR)
	{
		data = m_ppu_mask;
	}
	else if (addr == PPU_STATUS_ADDR) // status
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
		const u8 nametable = m_ppu_ctrl & Control::Flags::NAMETABLE_BASE_ADDR;
		// second part is masking to conserve the other parts that are not scroll
		m_t = ( nametable << 10 ) | ( m_t & ~0x0C00 );
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


void PPU::sprite_evaluation()
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
			m_current_oam_pos = 0;

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

void PPU::Serialize(std::fstream& fs)
{
	Fman::SerializeStatic(m_x);
	Fman::SerializeStatic(m_w);
	Fman::SerializeStatic(m_w);
	Fman::SerializeStatic(m_t);
	Fman::SerializeStatic(m_frames);
	Fman::SerializeStatic(m_scanline_done);
	Fman::SerializeStatic(m_frame_done);
	Fman::SerializeStatic(m_bg_next_tile_addr);
	Fman::SerializeStatic(m_bg_next_tile_attrib);
	Fman::SerializeStatic(m_bg_next_tile_hi);
	Fman::SerializeStatic(m_bg_next_tile_lo);
	Fman::SerializeStatic(m_bg_next_tile_id);
	Fman::SerializeStatic(m_dma_page);
	Fman::SerializeStatic(m_dma_addr);
	Fman::SerializeStatic(m_dma_data);
	Fman::SerializeStatic(m_dma_transfer);
	Fman::SerializeStatic(m_dma_dummy);
	Fman::SerializeStatic(m_bg_shifter_pattern_lo);
	Fman::SerializeStatic(m_bg_shifter_pattern_hi);
	Fman::SerializeStatic(m_bg_shifter_attrib_hi);
	Fman::SerializeStatic(m_bg_shifter_pattern_lo);
	Fman::SerializeStatic(m_secondary_oam_addr);
	Fman::SerializeStatic(m_secondary_oam_val);
	Fman::SerializeStatic(m_oam_copy_buffer);
	Fman::SerializeStatic(m_current_sprite_addr);
	Fman::SerializeStatic(m_current_oam_pos);
	Fman::SerializeStatic(m_current_sprite_count);
	Fman::SerializeStatic(m_added_sprite_0);
	Fman::SerializeStatic(m_oam_keep_copying);
	Fman::SerializeStatic(m_oam_sprite_bounded);
	Fman::SerializeStatic(m_oam_m);
	Fman::SerializeStatic(m_oam_n);
	Fman::SerializeStatic(m_scanline);
	Fman::SerializeStatic(m_cycle);
	Fman::SerializeStatic(m_data_buffer);
	Fman::SerializeStatic(m_oam_dma);
	Fman::SerializeStatic(m_oam_addr);
	Fman::SerializeStatic(m_ppu_ctrl.reg);
	Fman::SerializeStatic(m_ppu_status.reg);
	Fman::SerializeStatic(m_ppu_mask.reg);

	Fman::SerializeArrayStoresStatic<u16>(m_sprite_shifter_pattern_hi);
	Fman::SerializeArrayStoresStatic<u16>(m_sprite_shifter_pattern_lo);
	Fman::SerializeArrayStoresStatic<u8>(m_secondary_OAM);
	Fman::SerializeArrayStoresStatic<SpriteInfo>(m_scanline_sprites);
	Fman::SerializeArrayStoresStatic<ObjectAttributeEntry>(m_OAM);
	Fman::SerializeArrayStoresStatic<u8>(m_palette_ram_indexes);
	Fman::SerializeArrayStoresStatic<u32>(m_palettes_show);
	Fman::SerializeArrayStoresStatic<u8>(m_ram);
	Fman::SerializeArrayStoresStatic<u8>(m_pattern_tables);
	Fman::SerializeArrayStoresStatic<u32>({ m_pattern_tables_show, static_cast<size_t>( 128 * 128 * 2 ) });
	Fman::SerializeArrayStoresStatic<u32>({ m_screen, static_cast<size_t>( m_conf.width * m_conf.height ) });
}

void PPU::Deserialize(std::fstream& fs)
{
	Fman::DeserializeStatic(m_x);
	Fman::DeserializeStatic(m_w);
	Fman::DeserializeStatic(m_w);
	Fman::DeserializeStatic(m_t);
	Fman::DeserializeStatic(m_frames);
	Fman::DeserializeStatic(m_scanline_done);
	Fman::DeserializeStatic(m_frame_done);
	Fman::DeserializeStatic(m_bg_next_tile_addr);
	Fman::DeserializeStatic(m_bg_next_tile_attrib);
	Fman::DeserializeStatic(m_bg_next_tile_hi);
	Fman::DeserializeStatic(m_bg_next_tile_lo);
	Fman::DeserializeStatic(m_bg_next_tile_id);
	Fman::DeserializeStatic(m_dma_page);
	Fman::DeserializeStatic(m_dma_addr);
	Fman::DeserializeStatic(m_dma_data);
	Fman::DeserializeStatic(m_dma_transfer);
	Fman::DeserializeStatic(m_dma_dummy);
	Fman::DeserializeStatic(m_bg_shifter_pattern_lo);
	Fman::DeserializeStatic(m_bg_shifter_pattern_hi);
	Fman::DeserializeStatic(m_bg_shifter_attrib_hi);
	Fman::DeserializeStatic(m_bg_shifter_pattern_lo);
	Fman::DeserializeStatic(m_secondary_oam_addr);
	Fman::DeserializeStatic(m_secondary_oam_val);
	Fman::DeserializeStatic(m_oam_copy_buffer);
	Fman::DeserializeStatic(m_current_sprite_addr);
	Fman::DeserializeStatic(m_current_oam_pos);
	Fman::DeserializeStatic(m_current_sprite_count);
	Fman::DeserializeStatic(m_added_sprite_0);
	Fman::DeserializeStatic(m_oam_keep_copying);
	Fman::DeserializeStatic(m_oam_sprite_bounded);
	Fman::DeserializeStatic(m_oam_m);
	Fman::DeserializeStatic(m_oam_n);
	Fman::DeserializeStatic(m_scanline);
	Fman::DeserializeStatic(m_cycle);
	Fman::DeserializeStatic(m_data_buffer);
	Fman::DeserializeStatic(m_oam_dma);
	Fman::DeserializeStatic(m_oam_addr);
	Fman::DeserializeStatic(m_ppu_ctrl.reg);
	Fman::DeserializeStatic(m_ppu_status.reg);
	Fman::DeserializeStatic(m_ppu_mask.reg);

	Fman::DeserializeArrayStoresStatic<u16>(m_sprite_shifter_pattern_hi);
	Fman::DeserializeArrayStoresStatic<u16>(m_sprite_shifter_pattern_lo);
	Fman::DeserializeArrayStoresStatic<u8>(m_secondary_OAM);
	Fman::DeserializeArrayStoresStatic<SpriteInfo>(m_scanline_sprites);
	Fman::DeserializeArrayStoresStatic<ObjectAttributeEntry>(m_OAM);
	Fman::DeserializeArrayStoresStatic<u8>(m_palette_ram_indexes);
	Fman::DeserializeArrayStoresStatic<u32>(m_palettes_show);
	Fman::DeserializeArrayStoresStatic<u8>(m_ram);
	Fman::DeserializeArrayStoresStatic<u8>(m_pattern_tables);
	Fman::DeserializeArrayStoresStatic<u32>({ m_pattern_tables_show, static_cast<size_t>( 128 * 128 * 2 ) });
	Fman::DeserializeArrayStoresStatic<u32>({ m_screen, static_cast<size_t>( m_conf.width * m_conf.height ) });
}

void PPU::preload_tile()
{
	switch (( m_cycle - 1 ) % 8)
	{
	case 0: // tile id
		load_bg_shifters();
		m_bg_next_tile_id = memory_read(get_nametable_addr());
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
		m_bg_next_tile_attrib = memory_read(get_attribute_addr());
		if (( m_v >> 5 ) & 0x0002) m_bg_next_tile_attrib >>= 4;
		if (( m_v >> 0 ) & 0x0002) m_bg_next_tile_attrib >>= 2;
		m_bg_next_tile_attrib &= 0x03;
		break;
	case 4:
		m_bg_next_tile_addr =
			// offset background nametable address
			( ( m_ppu_ctrl & Control::Flags::BACKGROUND_PATTERN_ADDR ) << 8 )
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
		m_v++;
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

void PPU::update_shifters()
{
	if (can_show_background())
	{
		m_bg_shifter_pattern_lo <<= 1;
		m_bg_shifter_pattern_hi <<= 1;

		m_bg_shifter_attrib_lo <<= 1;
		m_bg_shifter_attrib_hi <<= 1;
	}
	if (can_show_sprites() && m_cycle <= 256 && m_cycle > 1)
	{
		for (size_t i = 0; i < m_current_sprite_count; i++)
		{
			if (m_scanline_sprites[i].x > 0)
			{
				m_scanline_sprites[i].x--;
			}
			else
			{
				m_sprite_shifter_pattern_lo[i] <<= 1;
				m_sprite_shifter_pattern_hi[i] <<= 1;
			}
		}
	}
}

// cycles 257 to 320
// every 2 cycles we copy a byte from secondary OAM
// to sprite shifters
void PPU::load_sprite_shifters()
{
	if (!is_rendering_enabled() || m_cycle >= 320)
	{
		return;
	}

	switch (m_cycle & 0x07)
	{
		// garbage NT
	case 1:
	{
		memory_read(0x2000 | ( m_v & 0x0FFF ));
		break;
	}
	// garbage NT
	case 3:
	{
		memory_read(0x2000 | ( m_v & 0x0FFF ));
		break;
	}
	// sprite lsbits
	case 5:
	{
		const auto y         = m_secondary_OAM[static_cast<size_t>( m_current_oam_pos * 4 + 0 )];
		const auto id        = m_secondary_OAM[static_cast<size_t>( m_current_oam_pos * 4 + 1 )];
		const auto attribute = m_secondary_OAM[static_cast<size_t>( m_current_oam_pos * 4 + 2 )];
		const auto x         = m_secondary_OAM[static_cast<size_t>( m_current_oam_pos * 4 + 3 )];
		u8 y_pos;
		// vertical mirroring
		if (attribute & 0x80)
		{
			y_pos = ( m_ppu_ctrl.is_flag_set(Control::Flags::LARGE_SPRITES) ? 15 : 7 ) - ( m_scanline - y );
		}
		else
		{
			y_pos = m_scanline - y;
		}

		m_scanline_sprites[m_current_oam_pos] = {
			attribute & 0x40 ? true : false,
			attribute & 0x20 ? false : true,
			static_cast<u8>( 4 + ( attribute & 0x03 ) ),
			static_cast<u8>( x ),
		};
		u16 base_addr;
		if (m_ppu_ctrl.is_flag_set(Control::Flags::LARGE_SPRITES))
		{
			base_addr =
				// base nametable address
				0x1000 * ( id & 0x01 )
				// sprite pos
				| 16 * ( ( id & ~0x01 ) >> 1 )
				// nametable y position
				| ( y_pos >= 8 ? y_pos + 8 : y_pos );
		}
		else
		{
			base_addr =
				// base nametable address
				( m_ppu_ctrl & Control::Flags::SPRITE_PATTERN_ADDR ) << 9
				// sprite pos
				| 16 * id
				// nametable y position
				| y_pos;
		}
		m_current_sprite_addr = base_addr;
		u8 lo_plane_bits = memory_read(m_current_sprite_addr);
		// http://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith64Bits
		if (m_scanline_sprites[m_current_oam_pos].flip_horizontally)
		{
			lo_plane_bits = BitReverseTable256[lo_plane_bits];
		}
		m_sprite_shifter_pattern_lo[m_current_oam_pos] = lo_plane_bits;
		break;
	}
	// sprite msbits
	case 7:
	{
		u8 hi_plane_bits = memory_read(m_current_sprite_addr + 8);
		if (m_scanline_sprites[m_current_oam_pos].flip_horizontally)
		{
			hi_plane_bits = BitReverseTable256[hi_plane_bits];
		}
		m_sprite_shifter_pattern_hi[m_current_oam_pos] = hi_plane_bits;
		m_current_oam_pos++;
		break;
	}
	default:
		break;
	}


}

u16 PPU::get_nametable_addr() const
{
	return 0x2000 | ( m_v & 0x0FFF );
}

u16 PPU::get_attribute_addr() const
{
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
	return 0x23C0
		| ( m_v & 0x0C00 )
		| ( ( m_v >> 4 ) & 0x38 )
		| ( ( m_v >> 2 ) & 0x07 );
}

u8 PPU::get_vram_increment_mode() const
{
	return m_ppu_ctrl.is_flag_set(Control::Flags::VRAM_INCREMENT) ? 32 : 1;
}

u8 PPU::get_sprite_y_size() const
{
	return m_ppu_ctrl.is_flag_set(Control::Flags::LARGE_SPRITES) ? 16 : 8;
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
	bool sprite_0_rendering = 0;
	u8 bg_pixel = 0;
	u8 bg_palette = 0;
	u8 sprite_pixel = 0;
	bool sprite_priority = false;
	u8 sprite_palette = 0;

	u8 pixel = 0;
	u8 palette = 0;
	if (can_show_background())
	{
		const u16 bit_mux = 0x8000 >> m_x;

		const u8 p0_pixel = ( m_bg_shifter_pattern_lo & bit_mux ) > 0;
		const u8 p1_pixel = ( m_bg_shifter_pattern_hi & bit_mux ) > 0;

		bg_pixel = ( p1_pixel << 1 ) | p0_pixel;

		const u8 bg_pal0 = ( m_bg_shifter_attrib_lo & bit_mux ) > 0;
		const u8 bg_pal1 = ( m_bg_shifter_attrib_hi & bit_mux ) > 0;

		bg_palette = ( bg_pal1 << 1 ) | bg_pal0;
	}
	if (can_show_sprites())
	{
		for (size_t i = 0; i < m_current_sprite_count; i++)
		{
			if (m_scanline_sprites[i].x != 0)
			{
				continue;
			}

			const u8 p0_pixel = ( m_sprite_shifter_pattern_lo[i] & 0x80 ) > 0;
			const u8 p1_pixel = ( m_sprite_shifter_pattern_hi[i] & 0x80 ) > 0;
			sprite_pixel = ( p1_pixel << 1 ) | p0_pixel;

			sprite_palette = m_scanline_sprites[i].palette;
			sprite_priority = m_scanline_sprites[i].priority;

			if (sprite_pixel != 0)
			{
				if (i == 0)
				{
					sprite_0_rendering = true;
				}
				break;
			}
		}
	}

	// priority calculation
	// ╔══════════╦══════════════╦══════════╦════════════════╗
	// ║ bg_pixel ║ sprite_pixel ║ priority ║     output     ║
	// ╠══════════╬══════════════╬══════════╬════════════════╣
	// │     0    │       0      │     X    │  EXT in $3F00  │
	// ├──────────┼──────────────┼──────────┼────────────────┤
	// │     0    │     1-3      │     X    │        SPRITE  │
	// ├──────────┼──────────────┼──────────┼────────────────┤
	// │   1-3    │       0      │     X    │            BG  │
	// ├──────────┼──────────────┼──────────┼────────────────┤
	// │   1-3    │     1-3      │     0    │         SPRITE │
	// ├──────────┼──────────────┼──────────┼────────────────┤
	// │   1-3    │     1-3      │     1    │             BG │
	// └──────────┴──────────────┴──────────┴────────────────┘
	if (bg_pixel == 0 && sprite_pixel == 0)
	{
		pixel = 0;
		palette = 0;
	}
	else if (bg_pixel == 0 && sprite_pixel != 0)
	{
		pixel = sprite_pixel;
		palette = sprite_palette;
	}
	else if (bg_pixel != 0 && sprite_pixel == 0)
	{
		pixel = bg_pixel;
		palette = bg_palette;
	}
	else if (bg_pixel != 0 && sprite_pixel != 0 && !sprite_priority)
	{
		pixel = bg_pixel;
		palette = bg_palette;
	}
	else if (bg_pixel != 0 && sprite_pixel != 0 && sprite_priority)
	{
		pixel = sprite_pixel;
		palette = sprite_palette;
	}

	if (sprite_0_rendering && m_added_sprite_0)
	{
		if (is_rendering_enabled())
		{
			if (!m_ppu_mask.is_flag_set(Mask::SHOW_LEFT_BACKGROUND | Mask::SHOW_LEFT_SPRITE))
			{
				if (m_cycle >= 9 && m_cycle < 257)
				{
					m_ppu_status.set_flags(Status::SPRITE_0_HIT);
				}
			}
			else
			{
				if (m_cycle >= 1 && m_cycle < 257)
				{
					m_ppu_status.set_flags(Status::SPRITE_0_HIT);
				}
			}
		}
	}

	return get_rendering_color(palette, pixel);
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
Color PPU::get_rendering_color(const u8 palette, const u8 pixel)
{
	const u8 index = m_palette_ram_indexes[static_cast<size_t>( palette * 4 + pixel )];
	if (m_ppu_mask & Mask::GRAYSCALE)
	{
		return m_palette[static_cast<size_t>( index & 0x30 )];
	}
	return m_palette[index];
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


}
