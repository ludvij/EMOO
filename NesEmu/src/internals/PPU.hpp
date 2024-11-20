#ifndef EMU_PPU_HEADER
#define EMU_PPU_HEADER

#include "Cartridge.hpp"
#include "Core.hpp"
#include "utils/Unreachable.hpp"
#include <array>
#include <bit>
#include <span>

namespace Emu
{

// your only purpose is to deal with endianness
struct Color
{
	Color() = default;
	Color(u8 val);
	Color(u8 R, u8 G, u8 B);
	u8 R;
	u8 G;
	u8 B;

	u32 AsU32() const;
	operator u32() const
	{
		return AsU32();
	}
};

// ordered as they appear in NES memory
struct ObjectAttributeEntry
{
	// Y pos of sprite
	u8 y{ 0xFF };
	// ID of tile from pattern
	u8 id{ 0 };
	// color flags and such
	u8 attribute{ 0 };
	// X pos of sprite
	u8 x{ 0 };
};

namespace Status
{

enum Flags : u8
{
	SPRITE_OVERFLOW = 0x20,
	SPRITE_0_HIT    = 0x40,
	VERTICAL_BLANK  = 0x80,
	ALL             = 0xE0,
};
}
namespace Control
{
enum Flags : u8
{
	NAMETABLE_BASE_ADDR     = 0x03,
	VRAM_INCREMENT          = 0x04,
	SPRITE_PATTERN_ADDR     = 0x08,
	BACKGROUND_PATTERN_ADDR = 0x10,
	LARGE_SPRITES           = 0x20,
	PPU_MASTER_SELECT       = 0x40,
	GENERATE_NMI            = 0x80,
	ALL                     = 0xFF,
};
}
namespace Mask
{
enum Flags : u8
{
	GRAYSCALE            = 0x01,
	SHOW_LEFT_BACKGROUND = 0x02,
	SHOW_LEFT_SPRITE     = 0x04,
	SHOW_BACKGROUND      = 0x08,
	SHOW_SPRITES         = 0x10,
	EMPHASIZE_RED        = 0x20,
	EMPHASIZE_GREEN      = 0x40,
	EMPHASIZE_BLUE       = 0x80,
	ALL                  = 0xFF,
};

}

// helper so I don't have to write the same 3 functions 12 times
struct RegisterFlags
{
	RegisterFlags(u8 val) : reg(val)
	{
	}
	operator u8() const
	{
		return reg;
	}
	bool is_flag_set(u8 flags) const;
	void set_flags(u8 flags, bool set=true);


	u8 reg;
};

// helper
struct SpriteInfo
{
	bool flip_horizontally{};
	bool priority{};
	u8 palette{};
	u8 x{};
};

class PPU
{
public:
	PPU(Configuration conf);
	~PPU();
	void Step();


	auto GetCycles() const
	{
		return m_cycle;
	}

	auto GetFrames() const
	{
		return m_frames;
	}

	bool IsScanlineDone() const
	{
		return m_cycle == 0;
	}

	auto GetScanlines() const
	{
		return m_scanline;
	}

	bool IsFrameDone() const;

	bool IsNMI() const;
	void SetNMI(bool set);

	bool IsDMATransfer() const;
	bool IsDMADummy() const;
	void SetDMADummy(bool set);
	void SetDMAData(u8 val);
	u16 GetDMAAddr() const;


	ObjectAttributeEntry GetOAMEntry(size_t entry) const;



	bool HasUpdatedPatternTables();
	bool HasUpdatedPalettes();

	void ConnectCartridge(std::shared_ptr<Cartridge> cartridge)
	{
		m_cartridge = cartridge;
	}

	void Reset();


	// not const some reads modify data
	u8 CpuRead(u16 addr);
	u8 CpuPeek(u16 addr) const;
	void CpuWrite(u16 addr, u8 val);

	void DMA();

	u8 X() const
	{
		return m_x;
	}
	u8 W() const
	{
		return m_w;
	}
	u16 V() const
	{
		return m_v;
	}
	u16 T() const
	{
		return m_t;
	}

	u32* GetScreen();

	u32* GetPatternTable(u8 idx, u8 palette);
	u32* GetPalette();
	Color GetColorFromPalette(u8 palette, u8 pixel);



private:

	u8 memory_read(u16 addr) const;
	void memory_write(u16 addr, u8 val);
	void write_ppu_addr(u8 val);
	void write_ppu_scroll(u8 val);


	void preload_tile();

	void increment_y();
	void increment_x();

	void reset_x();
	void reset_y();

	void load_bg_shifters();
	void update_shifters();
	void load_sprite_shifters();

	u16 get_nametable_addr() const;
	u16 get_attribute_addr() const;

	u8 get_vram_increment_mode() const;
	u8 get_sprite_y_size() const;

	bool can_show_background() const;
	bool can_show_sprites() const;
	bool can_show_background_or_sprites() const;

	u32 read_palette_ram_indexes(u16 addr) const;
	void write_palette_ram_indexes(u16 addr, u8 val);


	u32 get_color_for_pixel();
	Color get_rendering_color(u8 palette, u8 pixel);

	void draw_pixel();

	// quick hack to easily implement mirrorings
	enum class MirrorName
	{
		A, B, C, D
	};
	template<MirrorName m1, MirrorName m2, MirrorName m3, MirrorName m4>
	u8 nametable_mirrored_read(u16 addr) const;
	template<MirrorName m1, MirrorName m2, MirrorName m3, MirrorName m4>
	void nametable_mirrored_write(u16 addr, u8 val);

	void load_palette();

	void sprite_evaluation();

	bool is_rendering_enabled();


private:
	// for debugging purposes
	u32* m_pattern_tables_show{};
	// backup if nametables are not handled by the cartridge (unlikely)
	std::array<u8, 0x2000> m_pattern_tables{};

	// 2 nametables
	// other 2 are provided in cartridge
	std::array<u8, 2048> m_ram{};
	std::array<u32, 16 * 4> m_palettes_show{};

	u32* m_screen{};

	std::array<Color, 0x40> m_palette;
	//std::unordered_map<u16, u8> m_known_patterns;

	// not configurable palette inner ram
	// $3F00         -> Universal background color
	// $3F01 - $3F03 -> Background palette 0
	// $3F04         -> Unused color 1
	// $3F05 - $3F07 -> Background palette 1
	// $308          -> Unused color 2
	// $3F09 - $3F0B -> Background palette 2
	// $3F0C         -> Unused color 3
	// $3F0D - $3F0F -> Background palette 3
	// $3F10         -> Mirror of universal background color
	// $3F11 - $3F13 -> Sprite color 0
	// $3F14         -> Mirror of unused color 0
	// $3F15 - $3F17 -> Sprite color 1
	// $3F18         -> Mirror of unused color 1
	// $3F19 - $3F1B -> Sprite color 2
	// $3F1C         -> Mirror of unused color 2
	// $3F1D - $3F1F -> Sprite color 3
	std::array<u8, 0x0020> m_palette_ram_indexes{};
	//https://www.nesdev.org/wiki/PPU_palettes#2C02
	//TODO: add palette file handling instead of static array

	std::shared_ptr<Cartridge> m_cartridge{ nullptr };

	Configuration m_conf;

	// MMIO registers
	RegisterFlags m_ppu_ctrl   = 0b0000'0000;
	RegisterFlags m_ppu_mask   = 0b0000'0000;
	RegisterFlags m_ppu_status = 0b1010'0000; // power up state +0+x xxxx
	u8 m_oam_addr   = 0;
	//u8 m_oam_data     = 0;
	//u16 m_ppu_addr    = 0;
	u8 m_oam_dma = 0;

	u8 m_data_buffer = 0;

	constexpr static u16 PPU_CTRL_ADDR   = 0x2000;
	constexpr static u16 PPU_MASK_ADDR   = 0x2001;
	constexpr static u16 PPU_STATUS_ADDR = 0x2002;
	constexpr static u16 OAM_ADDR_ADDR   = 0x2003;
	constexpr static u16 OAM_DATA_ADDR   = 0x2004;
	constexpr static u16 PPU_SCROLL_ADDR = 0x2005;
	constexpr static u16 PPU_ADDR_ADDR   = 0x2006;
	constexpr static u16 PPU_DATA_ADDR   = 0x2007;

	constexpr static u16 OAM_DMA_ADDR    = 0x4014;

	// └┴ ─│
	// V and T are loopy registers
	// _yyy NNYY YYYX XXXX   ON RENDERING
	//  │││ ││││ │││└─┴┴┴┴──> coarse x scroll
	//  │││ ││└┴─┴┴┴────────> coarse y scroll
	//  │││ └┴──────────────> nametable select
	//  └┴┴─────────────────> fine y scroll
	// internal registers
	// during rendering:  used for the scroll position, 
	// outside rendering: used as the current VRAM address
	// this is the current VRAM address
	//   15 bits
	u16 m_v = 0;
	// during rendering:  starting coarse-x for the next scanline,
	//                    starting y scroll for the screen
	// outside rendering: holds the VRAM or scroll position before tranfering it
	//                    to V
	// this is the temporary VRAM address
	//   15 bits
	u16 m_t = 0;
	// the fine-x position of the current scroll
	//    3 bits
	u8 m_x = 0;
	// toggles on each write to either PPUSCROLL or PPUADDR, indicating wether is the first or
	// second write, clears on reads to PPUSTATUS
	//    1 bit
	u8 m_w = 0;


	u32 m_cycle = 0;
	u32 m_scanline = 0;

	// this was lifted from one lone coder
	ObjectAttributeEntry m_OAM[64];
	// ptr to oam so we can access it byte by byte
	u8* m_oam_data = reinterpret_cast<u8*>( m_OAM );

	u8 m_oam_n{ 0 };
	u8 m_oam_m{ 0 };
	bool m_oam_sprite_bounded{ false };
	bool m_oam_keep_copying{ false };
	bool m_added_sprite_0{ false };

	u8 m_current_sprite_count{ 0x00 };
	u8 m_current_oam_pos{ 0x00 };
	u16 m_current_sprite_addr{ 0x00 };

	std::array<SpriteInfo, 8> m_scanline_sprites;

	std::array<u8, 32> m_secondary_OAM;
	u8 m_secondary_oam_addr{ 0 };
	u8 m_secondary_oam_val{ 0 };
	u8 m_oam_copy_buffer{ 0 };

	u16 m_bg_shifter_pattern_lo{ 0x0000 };
	u16 m_bg_shifter_pattern_hi{ 0x0000 };
	u16 m_bg_shifter_attrib_lo{ 0x0000 };
	u16 m_bg_shifter_attrib_hi{ 0x0000 };

	std::array<u16, 8> m_sprite_shifter_pattern_lo{};
	std::array<u16, 8> m_sprite_shifter_pattern_hi{};

	u16 m_bg_next_tile_addr{ 0x00 };
	u8 m_bg_next_tile_attrib{ 0x00 };
	u8 m_bg_next_tile_id{ 0x00 };
	u8 m_bg_next_tile_lo{ 0x00 };
	u8 m_bg_next_tile_hi{ 0x00 };

	u8 m_dma_page{ 0x00 };
	u8 m_dma_addr{ 0x00 };
	u8 m_dma_data{ 0x00 };

	bool m_updated_patterns{ true };
	bool m_updated_palettes{ true };

	bool m_dma_transfer{ false };
	bool m_dma_dummy{ true };

	bool m_frame_done{ false };

	bool m_nmi{ false };

	bool m_scanline_done{ false };

	u32 m_frames{ 0 };

};

template <PPU::MirrorName m1, PPU::MirrorName m2, PPU::MirrorName m3, PPU::MirrorName m4>
u8 PPU::nametable_mirrored_read(const u16 addr) const
{
	// remove top 4 bits to access proper mirroring
	const u16 stripped_addr = addr & 0x0FFF;
	if (stripped_addr <= 0x03FF)
	{
		if      constexpr (m1 == MirrorName::A) return m_ram[stripped_addr & 0x3FF];
		else if constexpr (m1 == MirrorName::B) return m_ram[stripped_addr & 0x3FF + 0x400];
		else if constexpr (m1 == MirrorName::C) return *m_cartridge->PpuRead(addr);
		else if constexpr (m1 == MirrorName::D) return *m_cartridge->PpuRead(addr);
	}
	else if (0x0400 <= stripped_addr && stripped_addr <= 0x07FF)
	{
		if      constexpr (m2 == MirrorName::A) return m_ram[stripped_addr & 0x3FF];
		else if constexpr (m2 == MirrorName::B) return m_ram[stripped_addr & 0x3FF + 0x400];
		else if constexpr (m2 == MirrorName::C) return *m_cartridge->PpuRead(addr);
		else if constexpr (m2 == MirrorName::D) return *m_cartridge->PpuRead(addr);
	}
	else if (0x0800 <= stripped_addr && stripped_addr <= 0x0BFF)
	{
		if      constexpr (m3 == MirrorName::A) return m_ram[stripped_addr & 0x3FF];
		else if constexpr (m3 == MirrorName::B) return m_ram[stripped_addr & 0x3FF + 0x400];
		else if constexpr (m3 == MirrorName::C) return *m_cartridge->PpuRead(addr);
		else if constexpr (m3 == MirrorName::D) return *m_cartridge->PpuRead(addr);
	}
	else if (0x0C00 <= stripped_addr && stripped_addr <= 0x0FFF)
	{
		if      constexpr (m4 == MirrorName::A) return m_ram[stripped_addr & 0x3FF];
		else if constexpr (m4 == MirrorName::B) return m_ram[stripped_addr & 0x3FF + 0x400];
		else if constexpr (m1 == MirrorName::C) return *m_cartridge->PpuRead(addr);
		else if constexpr (m1 == MirrorName::D) return *m_cartridge->PpuRead(addr);
	}
	Lud::Unreachable();
}
template <PPU::MirrorName m1, PPU::MirrorName m2, PPU::MirrorName m3, PPU::MirrorName m4>
inline void PPU::nametable_mirrored_write(const u16 addr, const u8 val)
{
	// remove top 4 bits to access proper mirroring
	const u16 stripped_addr = addr & 0x0FFF;
	if (stripped_addr <= 0x03FF)
	{
		if      constexpr (m1 == MirrorName::A) m_ram[stripped_addr & 0x3FF] = val;
		else if constexpr (m1 == MirrorName::B) m_ram[stripped_addr & 0x3FF + 0x400] = val;
		else if constexpr (m1 == MirrorName::C) m_cartridge->PpuWrite(addr, val);
		else if constexpr (m1 == MirrorName::D) m_cartridge->PpuWrite(addr, val);
	}
	else if (0x0400 <= stripped_addr && stripped_addr <= 0x07FF)
	{
		if      constexpr (m2 == MirrorName::A) m_ram[stripped_addr & 0x3FF] = val;
		else if constexpr (m2 == MirrorName::B) m_ram[stripped_addr & 0x3FF + 0x400] = val;
		else if constexpr (m2 == MirrorName::C) m_cartridge->PpuWrite(addr, val);
		else if constexpr (m2 == MirrorName::D) m_cartridge->PpuWrite(addr, val);
	}
	else if (0x0800 <= stripped_addr && stripped_addr <= 0x0BFF)
	{
		if      constexpr (m3 == MirrorName::A) m_ram[stripped_addr & 0x3FF] = val;
		else if constexpr (m3 == MirrorName::B) m_ram[stripped_addr & 0x3FF + 0x400] = val;
		else if constexpr (m3 == MirrorName::C) m_cartridge->PpuWrite(addr, val);
		else if constexpr (m3 == MirrorName::D) m_cartridge->PpuWrite(addr, val);
	}
	else if (0x0C00 <= stripped_addr && stripped_addr <= 0x0FFF)
	{
		if      constexpr (m4 == MirrorName::A) m_ram[stripped_addr & 0x3FF] = val;
		else if constexpr (m4 == MirrorName::B) m_ram[stripped_addr & 0x3FF + 0x400] = val;
		else if constexpr (m4 == MirrorName::C) m_cartridge->PpuWrite(addr, val);
		else if constexpr (m4 == MirrorName::D) m_cartridge->PpuWrite(addr, val);
	}
}
}

#endif