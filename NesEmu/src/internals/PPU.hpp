#ifndef EMU_PPU_HEADER
#define EMU_PPU_HEADER

#include "Core.hpp"
#include "Bus.hpp"
#include "Cartridge.hpp"
#include "utils/Unreachable.hpp"

namespace Emu
{

constexpr u32 NTSC_FRAMERATE = 60;
constexpr u32 PAL_FRAMERATE  = 50;


template<u16 address>
struct MemoryMappedRegister
{
	MemoryMappedRegister() = default;
	void Link(Bus* bus) { m_bus = bus; }

	consteval u16 Address() const { return address; }

	u8   Get() const       { return m_bus->Read(address); }
	void Set(const u8 val) { m_bus->Write(address, val);  }

	u8   operator()() const       { return Get(); }
	void operator()(const u8 val) { Set(val);     }

	// I don't want to reassign this, ever
	MemoryMappedRegister(const MemoryMappedRegister&)            = delete;
	MemoryMappedRegister& operator=(const MemoryMappedRegister&) = delete;
private:
	Bus* m_bus = nullptr;
};

class Bus;
/*
 * 
 *          ___  ___
 *         |*  \/   |
 * R/W  >01]        [40< VCC
 * D0   [02]        [39> ALE
 * D1   [03]        [38] AD0
 * D2   [04]        [37] AD1
 * D3   [05]        [36] AD2
 * D4   [06]        [35] AD3
 * D5   [07]        [34] AD4
 * D6   [08]        [33] AD5
 * D7   [09]        [32] AD6
 * A2   >10]  2C02  [31] AD7
 * A1   >11]        [30> A8
 * A0   >12]        [29> A9
 * /CS  >13]        [28> A10
 * EXT1 [14]        [27> A11
 * EXT2 [15]        [26> A12
 * EXT3 [16]        [25> A13
 * EXT4 [17]        [24> /R
 * CLK  >18]        [23> /W
 * /VBL <19]        [22< /SYNC
 * VEE  >20]        [21> VOUT
 *         |        |
 *          --------
 */
class PPU
{
public:
	void ConnectBus(Bus* bus);

	void Step();

	void ConnectCartridge(const std::shared_ptr<Cartridge>& cartridge) { m_cartridge = cartridge; }

public:
	/*
	 * VPHB SINN
	 * ││││ ││└┴> Base Nametable address 
	 * |||| ||    (0 = $2000, 1 = $2400, 2 = $2800, 3 = $2C00)
	 * ││││ │└──> VRAM address increment per CPU read/write of PPUDATA
	 * |||| |     (0: add 1, going across; 1: add 32, going down)
	 * ││││ └-──> Sprite pattern table address for 8x8 sprites
	 * ||||       (0: $0000; 1 = $1000; ignored in 8x16)
	 * │││└─────> Background pattern table address
	 * |||        (0: $0000; 1 = $1000)
	 * ││└──────> Sprite size
	 * ||         (0: 8x8; 1 = 16x16)
	 * │└───────> PPU master/slave select
	 * |          (0: read backdrop from EXT pins; 1: output color on EXT pins)
	 * └────────> Generate and NMI at the start of the vertical blanking interval
	 *            (0: off; 1: on)
	 */
	MemoryMappedRegister<0x2000> PPUCTRL;
	MemoryMappedRegister<0x2001> PPUMASK;
	MemoryMappedRegister<0x2002> PPUSTATUS;
	MemoryMappedRegister<0x2003> OAMADDR;
	MemoryMappedRegister<0x2004> OAMDATA;
	MemoryMappedRegister<0x2005> PPUSCROLL;
	MemoryMappedRegister<0x2006> PPUADDR;
	MemoryMappedRegister<0x2007> PPUDATA;
	MemoryMappedRegister<0x4014> OAMDMA;

private:
	// not const some reads modify data
	u8 cpuRead(u16 addr);
	void cpuWrite(u16 addr, u8 val);

	u8 memoryRead(u16 addr);
	void memoryWrite(u16 addr, u8 val);

	// quick hack to easily implement mirrorings
	enum class MirrorName {A, B, C, D};
	template<MirrorName m1, MirrorName m2, MirrorName m3, MirrorName m4>
	u8 nametableMirroringRead(u16 addr);

private:
	Bus* m_bus = nullptr;
	std::shared_ptr<Cartridge> m_cartridge;

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
	std::array<u8, 0x0020> m_pallete{0};
};

template <PPU::MirrorName m1, PPU::MirrorName m2, PPU::MirrorName m3, PPU::MirrorName m4>
u8 PPU::nametableMirroringRead(u16 addr)
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
}

#endif