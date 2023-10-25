#include "pch.hpp"
#include "Cartridge.hpp"

namespace Emu
{
void Cartridge::readPGRChunks()
{
	const u8 pgrRomSizeBase = m_header.pgrRamChunks;
	const u8 pgrMult = m_header.romExt;
	if ((pgrMult & 0x00ff) == 0x00ff)
	{
		// evaluated as 2^E * (MM*2 + 1)
		const u8 exp = (pgrRomSizeBase & 0b11111100) >> 2;
		const u8 mult = pgrRomSizeBase & 0b00000011;
		m_pgrRomChunks = std::pow(2, exp) * (mult * 2 + 1);
	}
	else
	{
		m_pgrRomChunks = pgrRomSizeBase | (pgrMult & 0x00ff) << 8;
	}
}
}
