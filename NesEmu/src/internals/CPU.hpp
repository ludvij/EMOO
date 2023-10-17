#pragma once

/*
* This file contains the definition of the 6502 CPU (ricoh RP2A03) 
*/


#include "Core.hpp"
#include <string_view>



namespace Emu 
{
/*
* Processor status (m_S) flags
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


class Bus;

class CPU
{
public:  // Public functions
	CPU();

	void ConnectBus(Bus* bus) {m_bus = bus; }

	void Step();

	void Reset();

	u8 A() { return m_A; }
	u8 X() { return m_X; }
	u8 Y() { return m_Y; }
	u8 S() { return m_S; }
	u16 PC() { return m_PC; }
	u8 P() { return m_P; }

	void SetA(u8 A) { m_A = A; }
	void SetX(u8 X) { m_X = X; }
	void SetY(u8 Y) { m_Y = Y; }
	void SetS(u8 S) { m_S = S; }
	void SetPC(u8 PC) { m_PC = PC; }
	void SetP(u8 P) { m_P = P; }

	u32 GetCycles() { return m_cycles; }
	
public:  // Public fields
	
private: // private functions
	// reads a byte from PC and incrementes PC afterwards
	u8 readByte();

	u8 readMemory(u16 addr) const;
	void writeMemory(u16 addr, u8 val) const;


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
	u16 addrACC(); // m_A

	/*
	* official opcodes oredered as shown in the datasheet (alphabetical order)
	* By functionality
	* ┌-----------------------┬-----┬-----┬-----┬-----┬-----┬-----┬-----------┐
	* | Load/store            | LDA | LDX | LDY | STA | STX | STY |           |
	* ├-----------------------┼-----┼-----┼-----┼-----┼-----┴-----┴-----------┤
	* | Register transfer     | TAX | TAY | TXA | TYA |                       |
	* ├-----------------------┼-----┼-----┼-----┼-----┼-----┬-----┬-----------┤
	* | Stack operations      | TSX | TXS | PHA | PHP | PLA | PLP |           |
	* ├-----------------------┼-----┼-----┼-----┼-----┼-----┴-----┴-----------┤
	* | Logical               | AND | EOR | ORA | BIT |                       |
	* ├-----------------------┼-----┼-----┼-----┼-----┼-----┬-----------------┤
	* | Arithmetic            | ADC | SBC | CMP | CPX | CPY |                 |
	* ├-----------------------┼-----┼-----┼-----┼-----┼-----┼-----┬-----------┤
	* | Increment & decrement | INC | INX | INY | DEC | DEX | DEY |           |
	* ├-----------------------┼-----┼-----┼-----┼-----┼-----┴-----┴-----------┤
	* | Shift                 | ASL | RSL | ROL | ROR |                       |
	* ├-----------------------┼-----┼-----┼-----┼-----┴-----------------------┤
	* | Jump & calls          | JPM | JSR | RTS |                             |
	* ├-----------------------┼-----┼-----┼-----┼-----┬-----┬-----┬-----┬-----┤
	* | Branches              | BCC | BCS | BEQ | BMI | BNE | BPL | BVC | BVS |
	* ├-----------------------┼-----┼-----┼-----┼-----┼-----┼-----┼-----┼-----┤
	* | Status flag changes   | CLC | CLD | CLI | CLV | SEC | SED | SEI |     |
	* ├-----------------------┼-----┼-----┼-----┼-----┴-----┴-----┴-----┴-----┤
	* | system functions      | BRK | NOP | RTI |                             |
	* └-----------------------┴-----┴-----┴-----┴-----------------------------┘
	*/
	void ADC(u16 addr); 
	void AND(u16 addr); 
	void ASL(u16 addr); 

	void BCC(u16 addr);
	void BCS(u16 addr); 
	void BEQ(u16 addr); 
	void BIT(u16 addr); 
	void BMI(u16 addr);
	void BNE(u16 addr); 
	void BPL(u16 addr); 
	void BRK(u16 addr); 
	void BVC(u16 addr);
	void BVS(u16 addr); 

	void CLC(u16 addr); 
	void CLD(u16 addr); 
	void CLI(u16 addr);
	void CLV(u16 addr); 
	void CMP(u16 addr); 
	void CPX(u16 addr); 
	void CPY(u16 addr);

	void DEC(u16 addr); 
	void DEX(u16 addr); 
	void DEY(u16 addr); 

	void EOR(u16 addr);

	void INC(u16 addr); 
	void INX(u16 addr); 
	void INY(u16 addr); 

	void JMP(u16 addr);
	void JSR(u16 addr); 

	void LDA(u16 addr); 
	void LDX(u16 addr); 
	void LDY(u16 addr);
	void LSR(u16 addr); 

	void NOP(u16 addr); 

	void ORA(u16 addr); 

	void PHA(u16 addr);
	void PHP(u16 addr); 
	void PLA(u16 addr); 
	void PLP(u16 addr); 

	void ROL(u16 addr);
	void ROR(u16 addr); 
	void RTI(u16 addr); 
	void RTS(u16 addr); 

	void SBC(u16 addr);
	void SEC(u16 addr); 
	void SED(u16 addr); 
	void SEI(u16 addr); 
	void STA(u16 addr);
	void STX(u16 addr); 
	void STY(u16 addr); 

	void TAX(u16 addr); 
	void TAY(u16 addr);
	void TSX(u16 addr); 
	void TXA(u16 addr); 
	void TXS(u16 addr); 
	void TYA(u16 addr);

	// current workaround for unofficial opcodes
	[[noreturn]] 
	void ___(u16 addr);

	// some helper functions
	void setFlagIf(u8 flag, bool cond);
	bool isImplied() const;

	void branchIfCond(u16 addr, bool cond);
	void transferRegTo(u8 from, u8& to);


private: // private members
	Bus* m_bus = nullptr;

	u32 m_cycles = 0;
	u32 m_oopsCycles = 0;
	bool m_canOops = false;


	/*
	* This struct and array will act as the jump table in order to compile and store all the
	* exec instructions in the system.
	* Each element contains the following:
	*	an addrMode that acts as the addressing mode used in the instruction
	*	an exec that acts as the operation itself
	*	and the number of m_cycles the operation will take
	*/

	typedef u16 (CPU::*addrMode)();
	typedef void (CPU::*exec)(u16);
	struct Instruction
	{
		std::string_view name;
		addrMode addrMode = nullptr;
		exec exec = nullptr;
		u8 cycles = 0;
	};

	Instruction m_jumpTable[256];

	// Accumulator
	u8 m_A = 0;

	// Index registers
	u8 m_X = 0;
	u8 m_Y = 0;

	// Program counter
	u16 m_PC = 0;

	// Stack Pointer
	u8 m_S = 0;

	// Status register
	u8 m_P = 0;

	// some helpers
	u8 m_opcode = 0;
	Instruction m_currentInstr = {};
	// some operations do not save the result, I'll be saving them for
	// utility reasons
	u8 m_discard = 0;
};

}
