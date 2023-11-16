#ifndef EMU_MAPPER_HEADER
#define EMU_MAPPER_HEADER

#include "internals/Core.hpp"

#include <optional>

namespace Emu
{
class IMapper
{
public:
	IMapper(const u8 prgBanks, const u8 chrBanks) : m_prgBanks(prgBanks), m_chrBanks(chrBanks) {}
	virtual ~IMapper() = default;

	virtual [[nodiscard]] std::optional<u16> CpuMapRead (const u16 addr) const = 0;
	virtual [[nodiscard]] std::optional<u16> CpuMapWrite(const u16 addr) const = 0;

	virtual [[nodiscard]] std::optional<u16> PpuMapRead (const u16 addr) const = 0;
	virtual [[nodiscard]] std::optional<u16> PpuMapWrite(const u16 addr) const = 0;
	
protected:
	u8 m_prgBanks;
	u8 m_chrBanks;
};
} // namespace Emu



#endif