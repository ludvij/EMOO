#pragma once

/*
 * This file contains the definition of the 6502 CPU (ricoh RP2A03) 
*/

#include "Core.hpp"

#include <functional>

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

/*
* NES runs at 1.79 Mhz
* That means each clock cycle should take 1/1790000 seconds
* We get that signal by dividing the master clock
*/
constexpr u32 NTSC_CPU_FREQUENCY = NTSC_MASTER_CLOCK_SIGNAL / NTSC_CLOCK_DIVISOR;
constexpr u32  PAL_CPU_FREQUENCY =  PAL_MASTER_CLOCK_SIGNAL /  PAL_CLOCK_DIVISOR;

class CPU
{
public:  // Public functions
	CPU();
	
public:  // Public fields
	
private: // private functions
	void fillJumpTable() const;
private: // private members


	struct Instruction
	{
		std::function<void(Byte)> addrMode;
		std::function<Word(void)> opcode;
		Byte cycles;
	};

	static Instruction s_jumpTable[256];

	// Accumulator
	Byte m_A;

	// Index registers
	Byte m_X, m_Y;

	// Program counter
	Word m_PC;

	// Stack Pointer
	Byte m_S;

	// Status register
	Byte m_P;
};

}