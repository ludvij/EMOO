#ifndef EMU_CARTRIDGE_HEADER
#define EMU_CARTRIDGE_HEADER

#include "Core.hpp"

#include <array>
#include <string_view>
#include <memory>

namespace Emu
{
/*
 * https://www.nesdev.org/wiki/INES
 */
struct iNesHeader
{
	char name[4];
	u8 prgRomChunks;
	u8 chrRomChunks;
	u8 mapper1;
	u8 mapper2;
	u8 prgRamSize;
	u8 tv1;
	u8 tv2;
	u8 unused[5];

};


enum class Mirroring
{
	VERTICAL,
	HORIZONTAL,
};
	

class IMapper;

class Cartridge
{
public:
	explicit Cartridge(const std::string& filePath);

	void ConnectBus(Bus* bus) { m_bus = bus; }

	u8   CpuRead(u16 addr) const;
	void CpuWrite(u16 addr, u8 val);

	u8   PpuRead(u16 addr) const;
	void PpuWrite(u16 addr, u8 val);
private:


private:

	Bus* m_bus = nullptr;

	iNesHeader m_header = {};
	bool m_valid;

	std::unique_ptr<IMapper> m_mapper;

	Mirroring m_mirroring;
	u16 m_mapperNumber = 0;

	std::vector<u8> m_prgRom;
	std::vector<u8> m_chrRom;
};
}


#endif