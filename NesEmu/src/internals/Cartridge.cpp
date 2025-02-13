#include "pch.hpp"
#include "Cartridge.hpp"

#include "mappers/NROM.hpp"

namespace Emu
{

Cartridge::Cartridge(const std::filesystem::path& filePath)
	: m_valid(false)
	, m_file_path(filePath)
{
	std::filesystem::path path = filePath;
	std::ifstream inputFile;
	inputFile.open(path, std::ios::binary);
	m_name = path.stem().generic_string();

	if (!inputFile.is_open())
	{
		std::throw_with_nested(std::runtime_error("File not found"));
	}
	inputFile.read(std::bit_cast<char*>( &m_header ), sizeof m_header);


	// validate header
	if (!is_header_valid())
	{
		throw std::runtime_error("File is not a valid ROM");
	}
	// skip trainer if present
	if (m_header.flags_6 & 0x04)
	{
		inputFile.seekg(512, std::ios_base::cur);
	}

	// prg rom is in 16 kiB chunks
	m_prgRom.resize(static_cast<size_t>( m_header.prg_rom_chunks ) * 0x4000);
	inputFile.read(std::bit_cast<char*>( m_prgRom.data() ), m_prgRom.size() * sizeof u8);
	// chr rom is in 8 kib chunks
	m_chrRom.resize(static_cast<size_t>( m_header.chr_rom_chunks ) * 0x2000);
	inputFile.read(std::bit_cast<char*>( m_chrRom.data() ), m_chrRom.size() * sizeof u8);

	m_mapperNumber = ( m_header.flags_7 & 0xf0 ) | ( m_header.flags_6 >> 4 );
	m_mirroring = m_header.flags_6 & 0b1 ? Mirroring::Vertical : Mirroring::Horizontal;


	switch (m_mapperNumber)
	{
	case 0:
		m_mapper = std::make_unique<NROM>(m_header.prg_rom_chunks, m_header.chr_rom_chunks);
		break;
	default:
		std::println("Mapper [{:d}] Not implemented", m_mapperNumber);
		throw std::runtime_error("Mapper not implemented");
	}

	inputFile.close();
	m_valid = true;
	std::println(
		"Loaded ROM [{:s}]\n  Mapper:\n    [{:s} : {:d}]\n  Chunks:\n    [PGR : {:d}]\n    [CHR : {:d}]\n  Mirorring:\n    [{:s}]",
		filePath.generic_string(),
		m_mapper->GetName(),
		m_mapperNumber,
		m_header.prg_rom_chunks,
		m_header.chr_rom_chunks,
		to_string(m_mirroring)
	);
}

Cartridge::Cartridge(std::string_view name, const u8* data, const size_t size)
	: m_valid(false)
	, m_file_path("Reading from memory")
	, m_name(name)
{
	const u8* ptr_data = data;
	size_t offset = 0;
	std::memcpy(&m_header, ptr_data, sizeof iNesHeader);
	offset += sizeof iNesHeader;
	ptr_data = data + offset;

	// validate header
	if (!is_header_valid())
	{
		throw std::runtime_error("File is not a valid ROM");
	}

	// skip trainer if present
	if (m_header.flags_6 & 0b100)
	{
		offset += 512;
		ptr_data = data + offset;

	}

	// prg rom is in 16 kiB chunks
	m_prgRom.resize(static_cast<size_t>( m_header.prg_rom_chunks ) * 0x4000);
	std::memcpy(std::bit_cast<char*>( m_prgRom.data() ), ptr_data, m_prgRom.size() * sizeof u8);
	// chr rom is in 8 kib chunks
	offset += m_prgRom.size() * sizeof u8;
	ptr_data = data + offset;
	m_chrRom.resize(static_cast<size_t>( m_header.chr_rom_chunks ) * 0x2000);
	std::memcpy(std::bit_cast<char*>( m_chrRom.data() ), ptr_data, m_chrRom.size() * sizeof u8);

	m_mapperNumber = ( m_header.flags_7 & 0xf0 ) | ( m_header.flags_6 >> 4 );
	m_mirroring = m_header.flags_6 & 0b1 ? Mirroring::Vertical : Mirroring::Horizontal;


	switch (m_mapperNumber)
	{
	case 0:
		m_mapper = std::make_unique<NROM>(m_header.prg_rom_chunks, m_header.chr_rom_chunks);
		break;
	default:
		std::println("Mapper [{:d}] Not implemented", m_mapperNumber);
		throw std::runtime_error("Mapper not implemented");
	}

	m_valid = true;
	std::println(
		"Loaded ROM [{:s}]\n  Mapper:\n    [{:s} : {:d}]\n  Chunks:\n    [PGR : {:d}]\n    [CHR : {:d}]\n  Mirorring:\n    [{:s}]",
		m_file_path.generic_string(),
		m_mapper->GetName(),
		m_mapperNumber,
		m_header.prg_rom_chunks,
		m_header.chr_rom_chunks,
		to_string(m_mirroring)
	);
}

Cartridge::~Cartridge()
{
	std::println("  Unloaded ROM [{:s}]", m_file_path.generic_string());
}

std::optional<u8> Cartridge::CpuRead(u16 addr) const
{
	if (const auto mappedAddr = m_mapper->CpuMapRead(addr); mappedAddr)
	{
		return m_prgRom[*mappedAddr];
	}
	else
	{
		return std::nullopt;
	}

}

bool Cartridge::CpuWrite(u16 addr, u8 val)
{
	if (const auto mappedAddr = m_mapper->CpuMapWrite(addr); mappedAddr)
	{
		m_prgRom[*mappedAddr] = val;
		return true;
	}
	else
	{
		return false;
	}
}

std::optional<u8> Cartridge::PpuRead(u16 addr) const
{
	if (const auto mappedAddr = m_mapper->PpuMapRead(addr); mappedAddr)
	{
		return m_chrRom[*mappedAddr];
	}
	else
	{
		return std::nullopt;
	}
}

bool Cartridge::PpuWrite(u16 addr, u8 val)
{
	if (const auto mappedAddr = m_mapper->PpuMapRead(addr); mappedAddr)
	{
		m_chrRom[*mappedAddr] = val;
		return true;
	}
	else
	{
		return false;
	}
}
// return the name of the ROM as plaintext
// the name of the ROM is defined as the name of the file
// without the path and extension
std::string Cartridge::GetROMName()
{
	return m_name;
}
std::string Cartridge::to_string(Mirroring mirroring)
{
	switch (mirroring)
	{
	case Emu::Cartridge::Mirroring::Vertical:   return "VERTICAL";
	case Emu::Cartridge::Mirroring::Horizontal: return "HORIZONTAL";
	default:                                    return "??????????";
	}
}
bool Cartridge::is_header_valid() const
{
	return strncmp(m_header.name, "NES\x1A", 4) == 0;
}

void Cartridge::Serialize(std::fstream& fs)
{
	m_mapper->Serialize(fs);
}
void Cartridge::Deserialize(std::fstream& fs)
{
	m_mapper->Deserialize(fs);
}
}
