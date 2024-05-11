#include "pch.hpp"
#include "NROM.hpp"


namespace Emu
{
NROM::NROM(const u8 prgBanks, const u8 chrBanks)
	: IMapper(prgBanks, chrBanks)
{

}
/*
 * the mirroring goes:
 * in case of 32KiB
 *   1 -- [$8000, $BFFF]
 *   2 -- [$C000, $FFFF]
 * in case of 16KiB
 *   1 -- [$8000, $BFFF]
 *   2 -- [$C000, $FFFF] mirrored to 1
 * extra bank in case of Family Basic (RAM)
 * 	 1 -- [$6000, $7FFF]
 *
 * in case of 16KiB we have $BFFF - $8000 = $3FFF size
 * so the addresses get mapped to addr & $3FFF
 *
 * in case of 32Kib we have $FFFF - $8000 = $7FFF size
 * so the addresses get mapped to addr & 0x7FFF
 */
std::optional<u16> NROM::CpuMapRead(const u16 addr) const
{
	// out of range
	if (addr < 0x8000)
	{
		return std::nullopt;
	}
	if (m_prgBanks == 1)
	{
		return addr & 0x3FFF;
	}
	else
	{

		return addr & 0x7FFF;
	}
}

std::optional<u16> NROM::CpuMapWrite(const u16 addr) const
{
	// has the same mapper so
	return CpuMapRead(addr);
}
/*
 * This has no map
 */
std::optional<u16> NROM::PpuMapRead(const u16 addr) const
{
	if (addr >= 0x2000)
	{

		return std::nullopt;
	}
	return addr;
}

std::optional<u16> NROM::PpuMapWrite(const u16 addr) const
{
	return std::nullopt;
}
std::string NROM::GetName()
{
	return "NROM";
}
}