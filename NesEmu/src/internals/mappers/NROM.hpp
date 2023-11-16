#ifndef EMU_NROM_HEADER
#define EMU_NROM_HEADER

#include "Mapper.hpp"
#include "internals/Core.hpp"

namespace Emu
{
/*
 * mapper 000 (NROM)
 * 
 */
class NROM : public IMapper
{
	NROM(u8 prgBanks, u8 chrBanks);

	virtual std::optional<u16> CpuMapRead (const u16 addr) const override;
	virtual std::optional<u16> CpuMapWrite(const u16 addr) const override;
	virtual std::optional<u16> PpuMapRead (const u16 addr) const override;
	virtual std::optional<u16> PpuMapWrite(const u16 addr) const override;
};
} // namespace Emu

#endif