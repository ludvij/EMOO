#ifndef EMU_CARTRIDGE_HEADER
#define EMU_CARTRIDGE_HEADER

#include "Core.hpp"

#include "FileManager/FileManager.hpp"
#include "mappers/Mapper.hpp"
#include <array>
#include <filesystem>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

namespace A6502
{
class Disassembler;
}


namespace Emu
{
class Bus;

class Cartridge : public Fman::ISerializable
{
public:
	explicit Cartridge(const std::filesystem::path& filePath);
	explicit Cartridge(std::string_view name, const u8* data, size_t size);
	~Cartridge();

	void ConnectBus(Bus* bus)
	{
		m_bus = bus;
	}

	std::optional<u8> CpuRead(u16 addr) const;
	bool CpuWrite(u16 addr, u8 val);

	std::optional<u8> PpuRead(u16 addr) const;
	bool PpuWrite(u16 addr, u8 val);

	std::string GetROMName();

	// Inherited via ISerializable
	void Serialize(std::fstream& fs) override;
	void Deserialize(std::fstream& fs) override;

public:
	/*
	* https://www.nesdev.org/wiki/INES
	*/
	struct iNesHeader
	{
		char name[4];
		u8 prg_rom_chunks;
		u8 chr_rom_chunks;
		u8 flags_6;
		u8 flags_7;
		u8 prg_ram_size;
		u8 flags_9;
		u8 flags_10;
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
	Mirroring GetMirroring() const
	{
		return m_mirroring;
	}



private:

	std::string to_string(Mirroring mirroring);
	bool is_header_valid() const;

private:

	Bus* m_bus = nullptr;

	iNesHeader m_header = {};
	bool m_valid;

	std::unique_ptr<IMapper> m_mapper;

	Mirroring m_mirroring;
	u16 m_mapperNumber = 0;

	std::vector<u8> m_prgRom;
	std::vector<u8> m_chrRom;

	std::filesystem::path m_file_path;
	std::string m_name;

	friend class A6502::Disassembler;


};
}


#endif