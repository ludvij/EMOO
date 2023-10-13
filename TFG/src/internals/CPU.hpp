#pragma once

/*
 * This file contains the definition of the 6502 CPU (ricoh RP2A03) 
*/

#include "Core.hpp"

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

class Bus;

class CPU
{
public:  // Public functions
	CPU();

	void ConnectBus(Bus* bus) {m_bus = bus; }

	void Step();
	
public:  // Public fields
	
private: // private functions
	void fillJumpTable() const;

	Byte readByte();
	Byte read(Word addr) const;
	void write(Word addr, Byte val) const;

	Word addrZPI();  // d
	Word addrZPIX(); // d,x address mode
	Word addrZPIY(); // d,y address mode
	Word addrABS();  // a
	Word addrABSX(); // a,x address mode
	Word addrABSY(); // a,y address mode
	Word addrIND();  // (a)
	Word addrINDX(); // (d,x) address mode
	Word addrINDY(); // (d),y address mode

	Word addrIMP(); //
	Word addrACC(); // A
	Word addrIMM(); // #v
	Word addrREL(); // label

private: // private members

	Bus* m_bus = nullptr;

	u32 m_cycles = 0;
	u32 m_oopsAddr = 0;
	u32 m_oopsOp = 0;

	/*
	* This struct and array will act as the jump table in order to compile and store all the
	* opcode instructions in the system.
	* Each element contains the following:
	*	an addrMode that acts as the addressing mode used in the instruction
	*	an opcode that acts as the operation itself
	*	and the number of cycles the operation will take
	*/
	struct Instruction
	{
		Word (*addrMode)(void) = nullptr;
		void (*opcode  )(Word) = nullptr;
		Byte cycles = 0;
	};

	static Instruction s_jumpTable[256];

	// Accumulator
	Byte m_A = 0;

	// Index registers
	Byte m_X = 0;
	Byte m_Y = 0;

	// Program counter
	Word m_PC = 0;

	// Stack Pointer
	Byte m_S = 0;

	// Status register
	Byte m_P = 0;

	// some helpers

};

}