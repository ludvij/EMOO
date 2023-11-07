#ifndef EMU_MAPPER_HEADER
#define EMU_MAPPER_HEADER

#include "Core.hpp"

namespace Emu
{
class IMapper
{
public:
	virtual u8 CpuRead(u16 addr) const = 0;
	virtual void CpuWrite(const u16 addr, const u8 val) = 0;

	virtual u8 CpuRead(const u16 addr) const = 0;
	virtual void CpuWrite(const u16 addr, const u8 val) = 0;
};
} // namespace Emu



#endif