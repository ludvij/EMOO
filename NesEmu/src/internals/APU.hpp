#ifndef EMU_APU_HEADER
#define EMU_APU_HEADER

#include <array>

#include "Core.hpp"

namespace Emu
{
class APU
{
public:
	void Reset();
	void Step();

	void CpuWrite(u16 addr, u8 val);
	u8 CpuRead(u16 addr);
private:
	// pulse    $4000-$4007
	std::array<u8, 8> m_pulse;
	// triangle $4008-$400B
	std::array<u8, 4> m_triangle;
	// Noise    $400C-$400F
	std::array<u8, 4> m_noise;
	// DMC      $4010-$4013
	std::array<u8, 4> m_dmc;
};
}
#endif