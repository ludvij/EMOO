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
 * ││││ └-──>  Decimal mode flag, not used in the NES
 * │││└─────>  Break flag (mostly unused)
 * ││└──────>  Unused flag (always set)
 * │└───────> oVerflow flag
 * └────────>  Negative flag
 */
constexpr u8 P_C_FLAG = 0b00000001;
constexpr u8 P_Z_FLAG = 0b00000010;
constexpr u8 P_I_FLAG = 0b00000100;
constexpr u8 P_D_FLAG = 0b00001000;
constexpr u8 P_B_FLAG = 0b00010000;
constexpr u8 P_1_FLAG = 0b00100000;
constexpr u8 P_V_FLAG = 0b01000000;
constexpr u8 P_N_FLAG = 0b10000000;

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

	void ConnectBus(Bus* bus) {bus = bus; }

	void Step();
	
public:  // Public fields
	
private: // private functions
	void fillJumpTable();

	u8 readByte();
	u8 read(u16 addr) const;
	void write(u16 addr, u8 val) const;

	u8 fetch();


	// addressing modes
	u16 addrIMP(); //
	u16 addrIMM(); // #v
	u16 addrZPI(); // d
	u16 addrZPX(); // d,x address mode
	u16 addrZPY(); // d,y address mode
	u16 addrABS(); // a
	u16 addrABX(); // a,x address mode
	u16 addrABY(); // a,y address mode
	u16 addrIND(); // (a)
	u16 addrINX(); // (d,x) address mode
	u16 addrINY(); // (d),y address mode
	u16 addrREL(); // label
	// not used
	u16 addrACC(); // A

	// official opcodes
	void ADC(); void AND(); void ASL(); void BCC();
	void BCS(); void BEQ(); void BIT(); void BMI();
	void BNE(); void BPL(); void BRK(); void BVC();
	void BVS(); void CLC(); void CLD(); void CLI();
	void CLV(); void CMP(); void CPX(); void CPY();
	void DEC(); void DEX(); void DEY(); void EOR();
	void INC(); void INX(); void INY(); void JMP();
	void JSR(); void LDA(); void LDX(); void LDY();
	void LSR(); void NOP(); void ORA(); void PHA();
	void PHP(); void PLA(); void PLP(); void ROL();
	void ROR(); void RTI(); void RTS(); void SBC();
	void SEC(); void SED(); void SEI(); void STA();
	void STX(); void STY(); void TAX(); void TAY();
	void TSX(); void TXA(); void TXS(); void TYA();

	// current workaround for unofficial opcodes
	void ___();


private: // private members

	Bus* bus = nullptr;

	u32 cycles = 0;
	u32 oopsCycles = 0;
	bool canOops = false;


	/*
	* This struct and array will act as the jump table in order to compile and store all the
	* exec instructions in the system.
	* Each element contains the following:
	*	an addrMode that acts as the addressing mode used in the instruction
	*	an exec that acts as the operation itself
	*	and the number of cycles the operation will take
	*/
	struct Instruction
	{
		u16 (CPU::*addrMode)(void) = nullptr;
		void (CPU::*exec)(void) = nullptr;
		u8 cycles = 0;
	};

	Instruction jumpTable[256];

	// Accumulator
	u8 A = 0;

	// Index registers
	u8 X = 0;
	u8 Y = 0;

	// Program counter
	u16 PC = 0;

	// Stack Pointer
	u8 S = 0;

	// Status register
	u8 P = 0;

	// some helpers
	u8 opcode = 0;
	u16 addr = 0;
	Instruction* currentInstr = nullptr;
};

}