#ifndef EMU_CARTRIDGE_HEADER
#define EMU_CARTRIDGE_HEADER

#include "Core.hpp"

#include <array>
#include <string_view>
#include <vector>
#include <memory>
#include <optional>
#include "mappers/Mapper.hpp"

namespace Emu
{

class Cartridge
{
public:
	explicit Cartridge(const std::string& filePath);

	void ConnectBus(Bus* bus) { m_bus = bus; }

	std::optional<u8> CpuRead(u16 addr) const;
	bool CpuWrite(u16 addr, u8 val);

	std::optional<u8> PpuRead(u16 addr) const;
	bool PpuWrite(u16 addr, u8 val);

public:
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
		Vertical,
		Horizontal,
		//SingleScreen,
		//FourScreen,
		//ThreeScreenVertical
	};
	Mirroring GetMirroring() const { return m_mirroring; }
	

	
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