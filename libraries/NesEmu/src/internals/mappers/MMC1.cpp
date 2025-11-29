#include "MMC1.hpp"
#include "pch.hpp"

namespace Emu
{
MMC1::MMC1(u8 prgBanks, u8 chrBanks)
	: IMapper(prgBanks, chrBanks)
{
}

std::optional<u16> MMC1::CpuMapRead(const u16 addr) const
{
	return std::nullopt;
}

std::optional<u16> MMC1::CpuMapWrite(const u16 addr) const
{
	return std::nullopt;
}

std::optional<u16> MMC1::PpuMapRead(const u16 addr) const
{
	return std::nullopt;
}

std::optional<u16> MMC1::PpuMapWrite(const u16 addr) const
{
	return std::nullopt;
}

std::string MMC1::GetName()
{
	return "MMC1";
}


void MMC1::Serialize(std::ostream& fs)
{
}
void MMC1::Deserialize(std::istream& fs)
{
}
}
