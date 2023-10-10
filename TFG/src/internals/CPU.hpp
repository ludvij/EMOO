#pragma once

#include "Core.hpp"
#include "cstdint"

namespace Emu {
/*
* Processor status (S) flags
* 7654 3210
* NV1B DIZC
* ││││ │││└>  Carry flag
* ││││ ││└─>  Zero flag
* ││││ │└──>  Interrupt (IRQ) disable flag
* ││││ └-──>  Decimal mode flag
* │││└─────>  Break flag (mostly unused)
* ││└──────>  Unused flag (always set)
* │└───────> oVerflow flag
* └────────>  Negative flag
*/
constexpr Byte S_C_FLAG = 0b00000001;
constexpr Byte S_Z_FLAG = 0b00000010;
constexpr Byte S_I_FLAG = 0b00000100;
constexpr Byte S_D_FLAG = 0b00001000;
constexpr Byte S_B_FLAG = 0b00010000;
constexpr Byte S_1_FLAG = 0b00100000;
constexpr Byte S_V_FLAG = 0b01000000;
constexpr Byte S_N_FLAG = 0b10000000;

class CPU
{
public:


private:
private:
	// Clock things
	uint32_t m_cycles;
	uint32_t m_skipCycles;

	// Accumulator
	Byte m_A;

	// Index registers
	Byte m_X, m_Y;

	// Program counter
	Word m_PC;

	// Stack Pointer
	Byte m_S = S_1_FLAG;

	// Status register
	Byte m_P;
};

}