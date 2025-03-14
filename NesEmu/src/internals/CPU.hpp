#ifndef EMU_CPU_HEADER
#define EMU_CPU_HEADER

/*
* This file contains the definition of the 6502 CPU (ricoh RP2A03)
*/


#include "Core.hpp"
#include <array>
#include <string_view>

#include "FileManager/FileManager.hpp"

namespace Emu
{
/*
 * Processor status (m_P) flags
 * 7654 3210
 * NV1B DIZC
 * ││││ │││└>  Carry flag
 * ││││ ││└─>  Zero flag
 * ││││ │└──>  Interrupt (IRQ) disable flag
 * ││││ └-──>  Decimal mode flag, not used in the NES
 * │││└─────>  Break flag (mostly unused)
 * ││└──────>  Unused flag (always pushed as 1)
 * │└───────> oVerflow flag
 * └────────>  Negative flag
 */

namespace ProcessorStatus
{

enum Flags : u8
{
	C = 0x01,
	Z = 0x02,
	I = 0x04,
	D = 0x08,
	B = 0x10,
	U = 0x20,
	V = 0x40,
	N = 0x80,
};
}


class Bus;

class CPU : public Fman::ISerializable
{
public:  // Public functions
	CPU();
	~CPU();
	void ConnectBus(Bus* bus)
	{
		m_bus = bus;
	}

	bool IsDone() const
	{
		return m_done;
	}

	void Step();

	void Reset();
	void IRQ();
	void NMI();

	u8 A() const
	{
		return m_A;
	}
	u8 X() const
	{
		return m_X;
	}
	u8 Y() const
	{
		return m_Y;
	}
	u8 S() const
	{
		return m_S;
	}
	u8 P() const
	{
		return m_P;
	}
	u16 PC() const
	{
		return m_PC;
	}

	void SetA(const u8 A)
	{
		m_A = A;
	}
	void SetX(const u8 X)
	{
		m_X = X;
	}
	void SetY(const u8 Y)
	{
		m_Y = Y;
	}
	void SetS(const u8 S)
	{
		m_S = S;
	}
	void SetP(const u8 P)
	{
		m_P = P;
	}
	void SetPC(const u16 PC)
	{
		m_PC = PC;
	}

	u32 GetCycles()      const
	{
		return m_cycle;
	}
	u32 GetTotalCycles() const
	{
		return m_totalCycles;
	}

	// Inherited via ISerializable
	void Serialize(std::fstream& fs) override;

	void Deserialize(std::fstream& fs) override;

public:
	constexpr static u16 STACK_VECTOR    = 0x0100;
	constexpr static u16 IRQ_VECTOR_LO   = 0xFFFE;
	constexpr static u16 IRQ_VECTOR_HI   = 0xFFFF;
	constexpr static u16 NMI_VECTOR_LO   = 0xFFFA;
	constexpr static u16 NMI_VECTOR_HI   = 0xFFFB;
	constexpr static u16 RESET_VECTOR_LO = 0xFFFC;
	constexpr static u16 RESET_VECTOR_HI = 0xFFFD;

private: // private functions
	u8 memoryRead(u16 addr) const;
	void memoryWrite(u16 addr, u8 val) const;
	// reads a byte from PC and incrementes PC afterwards
	u8 readByte();



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

	// illegal opcodes;
	// nops
	void SKB(u16 addr);
	void IGN(u16 addr);

	// combined operations
	void ALR(u16 addr);
	void ANC(u16 addr);
	void ARR(u16 addr);
	void AXS(u16 addr);
	void LAX(u16 addr);
	void SAX(u16 addr);

	// RMW instructions
	void DCP(u16 addr);
	void ISC(u16 addr);
	void RLA(u16 addr);
	void RRA(u16 addr);
	void SLO(u16 addr);
	void SRE(u16 addr);

	// utils
	void ADD(u8 val);

	// current workaround for unofficial opcodes
	void STP(u16 addr);

	// some helper functions
	void setFlagIf(u8 flag, bool cond);
	void setFlag(u8 flag);
	void clearFlag(u8 flag);
	bool checkFlag(u8 flag) const;
	bool isImplied() const;

	void branchIfCond(u16 addr, bool cond);
	void transferRegTo(u8 from, u8& to);
	void push(u8 val);
	void push_word(u16 val);
	u8 pop();
	u16 pop_word();

	void set_register(u8& reg, const u8 val);
	void set_p(u8 val);


private: // private members
	Bus* m_bus = nullptr;

	u32 m_cycle = 0;
	u32 m_oopsCycles = 0;
	u32 m_totalCycles = 0;
	bool m_canOops = false;


	/*
	* This struct and array will act as the jump table in order to compile and store all the
	* exec instructions in the system.
	* Each element contains the following:
	*	an addrMode that acts as the addressing mode used in the instruction
	*	an exec that acts as the operation itself
	*	and the number of m_cycle the operation will take
	*/
	typedef u16(CPU::* AddressingModeCallable)( );
	typedef void ( CPU::* InstructionCallable )( u16 );
	struct Instruction
	{
		const char* name = "STP";
		AddressingModeCallable addressing_mode_fn = nullptr;
		InstructionCallable instrucion_fn = nullptr;
		u8 cycles = 0;
	};

	std::array<Instruction, 256> m_jump_table;

	// Accumulator
	u8 m_A = 0;

	// Index registers
	u8 m_X = 0;
	u8 m_Y = 0;

	// Program counter
	u16 m_PC = 0;

	// Stack Pointer
	u8 m_S = 0;
	// vectors



	// Status register
	u8 m_P = 0;

	// some helpers
	u8 m_opcode = 0;
	Instruction m_current_instr = {};
	// some operations do not save the result, I'll be saving them for
	// utility reasons
	u8 m_discard = 0;

	bool m_done{ false };


};

}

#endif