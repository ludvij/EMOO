#include "pch.hpp"
#include "APU.hpp"

namespace Emu
{
void APU::Reset()
{
}
void APU::Step()
{
}
void APU::CpuWrite(u16 addr, u8 val)
{
}
u8 APU::CpuRead(u16 addr)
{
	return u8();
}
u8 APU::CpuPeek(u16 addr) const
{
	return u8();
}
}