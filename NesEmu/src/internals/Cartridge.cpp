#include "pch.hpp"
#include "Cartridge.hpp"

#include "mappers/NROM.hpp"

namespace Emu
{

Cartridge::Cartridge(const std::string& filePath)
	: m_valid(false)
{
	std::ifstream inputFile;
	inputFile.open(filePath, std::ios::binary);

	if (!inputFile.is_open())
	{
		std::throw_with_nested(std::runtime_error("File not found"));
	}
	inputFile.read(std::bit_cast<char*>(&m_header), sizeof(m_header));

	// validate header

	// skip trainer if present
	if (m_header.mapper1 & 0b100)
	{
		inputFile.seekg(512, std::ios_base::cur);
	}

	// prg rom is in 16 kiB chunks
	m_prgRom.resize(m_header.prgRomChunks * 16384);
	inputFile.read(std::bit_cast<char*>(m_prgRom.data()), static_cast<size_t>(m_prgRom.size()));
	// chr rom is in 8 kib chunks
	m_chrRom.resize(m_header.chrRomChunks * 8192);
	inputFile.read(std::bit_cast<char*>(m_chrRom.data()), static_cast<size_t>(m_chrRom.size()));

	m_mapperNumber = (m_header.mapper2 & 0xf0) | (m_header.mapper1 >> 4);
	m_mirroring = m_header.mapper1 & 0b1 ? Mirroring::VERTICAL : Mirroring::HORIZONTAL;


	switch (m_mapperNumber)
	{
	case 0:
		m_mapper = std::make_unique<NROM>(m_header.prgRomChunks, m_header.chrRomChunks);
		break;
	default:
		std::throw_with_nested(std::runtime_error("Mapper not implemented"));
	}

	inputFile.close();
	m_valid = true;
}

u8 Cartridge::CpuRead(u16 addr) const
{
	const u16 mappedAddr = m_mapper->CpuMapRead(addr);
	return m_prgRom[mappedAddr];
}

void Cartridge::CpuWrite(u16 addr, u8 val)
{
	const u16 mappedAddr = m_mapper->CpuMapWrite(addr);
	m_prgRom[mappedAddr] = val;
}
}
