#ifndef EMU_PPU_HEADER
#define EMU_PPU_HEADER

#include "Core.hpp"
#include "Cartridge.hpp"
#include "utils/Unreachable.hpp"

namespace Emu
{

constexpr u32 NTSC_FRAMERATE = 60;
constexpr u32 PAL_FRAMERATE  = 50;



class PPU
{
public:

	void Step();

	void ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge) { m_cartridge = cartridge; }

	// not const some reads modify data
	u8 CpuRead(u16 addr);
	void CpuWrite(u16 addr, u8 val);

private:

	u8 memoryRead(u16 addr);
	void memoryWrite(u16 addr, u8 val);
	void write_ppu_addr(u8 val);
	void write_ppu_scroll(u8 val);

	// quick hack to easily implement mirrorings
	enum class MirrorName {A, B, C, D};
	template<MirrorName m1, MirrorName m2, MirrorName m3, MirrorName m4>
	u8 nametableMirroringRead(u16 addr);
	template<MirrorName m1, MirrorName m2, MirrorName m3, MirrorName m4>
	u8 nametableMirroringWrite(u16 addr, u8 val);



private:
	std::shared_ptr<Cartridge> m_cartridge;

	// MMIO registers
	u8 m_ppu_ctrl     = 0;
	u8 m_ppu_status   = 0;
	u8 m_ppu_mask     = 0;
	u8 m_oam_addr     = 0;
	//u8 m_oam_data     = 0;
	u8 m_ppu_scroll_x = 0;
	u8 m_ppu_scroll_y = 0;
	//u16 m_ppu_addr    = 0;
	u8 m_ppu_data     = 0;
	u8 m_oam_dma      = 0;

	u8 m_data_buffer;

	// $2000 
	const u16 PPU_CTRL_ADDR   = 0x2000;
	// $2001 
	const u16 PPU_MASK_ADDR   = 0x2001;
	// $2002 
	const u16 PPU_STATUS_ADDR = 0x2002;
	// $2003 
	const u16 OAM_ADDR_ADDR   = 0x2003;
	// $2004 
	const u16 OAM_DATA_ADDR   = 0x2004;
	// $2005 
	const u16 PPU_SCROLL_ADDR = 0x2005;
	// $2006 
	const u16 PPU_ADDR_ADDR   = 0x2006;
	// $2007 
	const u16 PPU_DATA_ADDR   = 0x2007;
	// $4014 
	const u16 OAM_DMA_ADDR    = 0x4014;

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


	u32 m_cycles = 0;
	u32 m_scanlines = 0;

	
	// 2 nametables
	// other 2 are provided in cartridge
	std::array<u8, 2000> m_ram{0};
	// backup if nametables are not handled by the cartridge (unlikely)
	std::array<u8, 0x2000> m_patternTable{0};
	// not configurable palette inner ram
	// $3F00         -> Universal background color
	// $3F01 - $3F03 -> Background palette 0
	// $3F05 - $3F07 -> Background palette 1
	// $3F09 - $3F0B -> Background palette 2
	// $3F0D - $3F0F -> Background palette 3
	// $3F10         -> Mirror of universal background color
	// $3F11 - $3F13 -> Sprite color 0
	// $3F15 - $3F17 -> Sprite color 1
	// $3F19 - $3F1B -> Sprite color 2
	// $3F1D - $3F1F -> Sprite color 3
	std::array<u8, 0x0020> m_palleteRamIndexes{0};

	// this was lifted from one lone coder
	struct OAM_Data
	{
		u8 y;
		u8 id;
		u8 attribute;
		u8 x;
	} OAM[64];
	u8* m_oam_data = (u8*)OAM;
};

template <PPU::MirrorName m1, PPU::MirrorName m2, PPU::MirrorName m3, PPU::MirrorName m4>
u8 PPU::nametableMirroringRead(const u16 addr)
{
	// remove top 4 bits to access proper mirroring
	const u16 strippedAddr = addr & 0x0FFF;
	if (strippedAddr <= 0x03FF)
	{
		if      constexpr (m1 == MirrorName::A) return m_ram[strippedAddr & 0x3FF];
		else if constexpr (m1 == MirrorName::B) return m_ram[strippedAddr & 0x3FF + 0x400];
		else if constexpr (m1 == MirrorName::C) return *m_cartridge->PpuRead(addr);
		else if constexpr (m1 == MirrorName::D) return *m_cartridge->PpuRead(addr);
	}
	else if (0x0400 <= strippedAddr && strippedAddr <= 0x07FF)
	{
		if      constexpr (m2 == MirrorName::A) return m_ram[strippedAddr & 0x3FF];
		else if constexpr (m2 == MirrorName::B) return m_ram[strippedAddr & 0x3FF + 0x400];
		else if constexpr (m2 == MirrorName::C) return *m_cartridge->PpuRead(addr);
		else if constexpr (m2 == MirrorName::D) return *m_cartridge->PpuRead(addr);
	}
	else if (0x0800 <= strippedAddr && strippedAddr <= 0x0BFF)
	{
		if      constexpr (m3 == MirrorName::A) return m_ram[strippedAddr & 0x3FF];
		else if constexpr (m3 == MirrorName::B) return m_ram[strippedAddr & 0x3FF + 0x400];
		else if constexpr (m3 == MirrorName::C) return *m_cartridge->PpuRead(addr);
		else if constexpr (m3 == MirrorName::D) return *m_cartridge->PpuRead(addr);
	}
	else if (0x0C00 <= strippedAddr && strippedAddr <= 0x0FFF)
	{
		if      constexpr (m4 == MirrorName::A) return m_ram[strippedAddr & 0x3FF];
		else if constexpr (m4 == MirrorName::B) return m_ram[strippedAddr & 0x3FF + 0x400];
		else if constexpr (m1 == MirrorName::C) return *m_cartridge->PpuRead(addr);
		else if constexpr (m1 == MirrorName::D) return *m_cartridge->PpuRead(addr);
	}
	Lud::Unreachable();
}
template <PPU::MirrorName m1, PPU::MirrorName m2, PPU::MirrorName m3, PPU::MirrorName m4>
inline u8 PPU::nametableMirroringWrite(const u16 addr, const u8 val)
{
	// remove top 4 bits to access proper mirroring
	const u16 strippedAddr = addr & 0x0FFF;
	if (strippedAddr <= 0x03FF)
	{
		if      constexpr (m1 == MirrorName::A) m_ram[strippedAddr & 0x3FF] = val;
		else if constexpr (m1 == MirrorName::B) m_ram[strippedAddr & 0x3FF + 0x400] = val;
		else if constexpr (m1 == MirrorName::C) m_cartridge->PpuWrite(addr, val);
		else if constexpr (m1 == MirrorName::D) m_cartridge->PpuWrite(addr, val);
	}
	else if (0x0400 <= strippedAddr && strippedAddr <= 0x07FF)
	{
		if      constexpr (m1 == MirrorName::A) m_ram[strippedAddr & 0x3FF] = val;
		else if constexpr (m1 == MirrorName::B) m_ram[strippedAddr & 0x3FF + 0x400] = val;
		else if constexpr (m1 == MirrorName::C) m_cartridge->PpuWrite(addr, val);
		else if constexpr (m1 == MirrorName::D) m_cartridge->PpuWrite(addr, val);
	}
	else if (0x0800 <= strippedAddr && strippedAddr <= 0x0BFF)
	{
		if      constexpr (m1 == MirrorName::A) m_ram[strippedAddr & 0x3FF] = val;
		else if constexpr (m1 == MirrorName::B) m_ram[strippedAddr & 0x3FF + 0x400] = val;
		else if constexpr (m1 == MirrorName::C) m_cartridge->PpuWrite(addr, val);
		else if constexpr (m1 == MirrorName::D) m_cartridge->PpuWrite(addr, val);
	}
	else if (0x0C00 <= strippedAddr && strippedAddr <= 0x0FFF)
	{
		if      constexpr (m1 == MirrorName::A) m_ram[strippedAddr & 0x3FF] = val;
		else if constexpr (m1 == MirrorName::B) m_ram[strippedAddr & 0x3FF + 0x400] = val;
		else if constexpr (m1 == MirrorName::C) m_cartridge->PpuWrite(addr, val);
		else if constexpr (m1 == MirrorName::D) m_cartridge->PpuWrite(addr, val);
	}
	Lud::Unreachable();
}
}

#endif