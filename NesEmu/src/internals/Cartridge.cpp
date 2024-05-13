#include "pch.hpp"
#include "Cartridge.hpp"

#include "mappers/NROM.hpp"

namespace Emu
{

Cartridge::Cartridge(const std::string& filePath)
	: m_valid(false)
	, m_file_path(filePath)
{
	std::ifstream inputFile;
	inputFile.open(filePath, std::ios::binary);

	if (!inputFile.is_open())
	{
		std::throw_with_nested(std::runtime_error("File not found"));
	}
	inputFile.read(std::bit_cast<char*>( &m_header ), sizeof m_header);


	// validate header

	// skip trainer if present
	if (m_header.mapper1 & 0b100)
	{
		inputFile.seekg(512, std::ios_base::cur);
	}

	// prg rom is in 16 kiB chunks
	m_prgRom.resize(static_cast<size_t>( m_header.prgRomChunks ) * 0x4000);
	inputFile.read(std::bit_cast<char*>( m_prgRom.data() ), static_cast<size_t>( m_prgRom.size() ));
	// chr rom is in 8 kib chunks
	m_chrRom.resize(static_cast<size_t>( m_header.chrRomChunks ) * 0x2000);
	inputFile.read(std::bit_cast<char*>( m_chrRom.data() ), static_cast<size_t>( m_chrRom.size() ));

	m_mapperNumber = ( m_header.mapper2 & 0xf0 ) | ( m_header.mapper1 >> 4 );
	m_mirroring = m_header.mapper1 & 0b1 ? Mirroring::Vertical : Mirroring::Horizontal;


	switch (m_mapperNumber)
	{
	case 0:
		m_mapper = std::make_unique<NROM>(m_header.prgRomChunks, m_header.chrRomChunks);
		break;
	default:
		std::println("Mapper [{:d}] Not implemented", m_mapperNumber);
		std::throw_with_nested(std::runtime_error("Mapper not implemented"));
	}

	inputFile.close();
	m_valid = true;
	std::println(
		"Loaded ROM [{:s}]\n  Mapper:\n    [{:s} : {:d}]\n  Chunks:\n    [PGR : {:d}]\n    [CHR : {:d}]\n  Mirorring:\n    [{:s}]",
		filePath,
		m_mapper->GetName(),
		m_mapperNumber,
		m_header.prgRomChunks,
		m_header.chrRomChunks,
		to_string(m_mirroring)
	);
}

Cartridge::~Cartridge()
{
	std::println("  Unloaded ROM [{:s}]", m_file_path);
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
	if (const auto mappedAddr = m_mapper->PpuMapWrite(addr); mappedAddr)
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
std::string Cartridge::to_string(Mirroring mirroring)
{
	switch (mirroring)
	{
	case Emu::Cartridge::Mirroring::Vertical:   return "VERTICAL";
	case Emu::Cartridge::Mirroring::Horizontal: return "HORIZONTAL";
	default:                                    return "??????????";
	}
}
}
