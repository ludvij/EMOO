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

	// quick hack to easily implement mirrorings
	enum class MirrorName {A, B, C, D};
	template<MirrorName m1, MirrorName m2, MirrorName m3, MirrorName m4>
	u8 nametableMirroringRead(u16 addr);
	template<MirrorName m1, MirrorName m2, MirrorName m3, MirrorName m4>
	u8 nametableMirroringWrite(u16 addr, u8 val);

private:
	std::shared_ptr<Cartridge> m_cartridge;

	u8 m_ppu_ctrl    = 0;
	u8 m_ppu_status  = 0;
	u8 m_ppu_mask    = 0;
	u8 m_oam_addr    = 0;
	u8 m_oam_data    = 0;
	u16 m_ppu_scroll = 0;
	u16 m_ppu_addr   = 0;
	u8 m_ppu_data    = 0;
	u8 m_oam_dma     = 0;

	u32 m_cycles = 0;
	u32 m_scanlines = 0;

	u8 m_address_latch = 0;
	
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