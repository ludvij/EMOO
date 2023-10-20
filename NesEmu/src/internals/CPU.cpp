#include "pch.hpp"

#include "CPU.hpp"

#include "Bus.hpp"

#define MAKE_WORD(hi, lo) (((hi) << 8) | (lo))

#define CHECK_BIT(val, bit) (((val) >> (bit)) & 1)

#define CHECK_FLAG(flag) ((m_P & (flag)) ? 1 : 0)

namespace Emu 
{


CPU::CPU()
{
	// filling the jump table
	Instruction invalid = {
		"___",
		&CPU::addrIMM, 
		&CPU::___, 
		0
	};
	std::fill_n(m_jumpTable, 256, invalid);
	// ADC
	m_jumpTable[0x69] = {"ADC", &CPU::addrIMM, &CPU::ADC, 2};
	m_jumpTable[0x65] = {"ADC", &CPU::addrZPI, &CPU::ADC, 3};
	m_jumpTable[0x75] = {"ADC", &CPU::addrZPX, &CPU::ADC, 4};
	m_jumpTable[0x6D] = {"ADC", &CPU::addrABS, &CPU::ADC, 4};
	m_jumpTable[0x7D] = {"ADC", &CPU::addrABX, &CPU::ADC, 4};
	m_jumpTable[0x79] = {"ADC", &CPU::addrABY, &CPU::ADC, 4};
	m_jumpTable[0x61] = {"ADC", &CPU::addrINX, &CPU::ADC, 6};
	m_jumpTable[0x71] = {"ADC", &CPU::addrINY, &CPU::ADC, 5};
	// AND
	m_jumpTable[0x29] = {"AND", &CPU::addrIMM, &CPU::AND, 2};
	m_jumpTable[0x25] = {"AND", &CPU::addrZPI, &CPU::AND, 3};
	m_jumpTable[0x35] = {"AND", &CPU::addrZPX, &CPU::AND, 4};
	m_jumpTable[0x2D] = {"AND", &CPU::addrABS, &CPU::AND, 4};
	m_jumpTable[0x3D] = {"AND", &CPU::addrABX, &CPU::AND, 4};
	m_jumpTable[0x39] = {"AND", &CPU::addrABY, &CPU::AND, 4};
	m_jumpTable[0x21] = {"AND", &CPU::addrINX, &CPU::AND, 6};
	m_jumpTable[0x31] = {"AND", &CPU::addrINY, &CPU::AND, 5};
	// ASL
	m_jumpTable[0x0A] = {"ASL", &CPU::addrACC, &CPU::ASL, 2};
	m_jumpTable[0x06] = {"ASL", &CPU::addrZPI, &CPU::ASL, 5};
	m_jumpTable[0x0E] = {"ASL", &CPU::addrABS, &CPU::ASL, 6};
	m_jumpTable[0x16] = {"ASL", &CPU::addrZPX, &CPU::ASL, 6};
	m_jumpTable[0x1A] = {"ASL", &CPU::addrABX, &CPU::ASL, 7};
	// BCC
	m_jumpTable[0x90] = {"BCC", &CPU::addrREL, &CPU::BCC, 2};
	// BCS
	m_jumpTable[0xB0] = {"BCS", &CPU::addrREL, &CPU::BCS, 2};
	// BEQ
	m_jumpTable[0xF0] = {"BEQ", &CPU::addrREL, &CPU::BEQ, 2};
	// BIT
	m_jumpTable[0x24] = {"BIT", &CPU::addrZPI, &CPU::BIT, 3};
	m_jumpTable[0x2C] = {"BIT", &CPU::addrABS, &CPU::BIT, 4};
	// BMI
	m_jumpTable[0x30] = {"BMI", &CPU::addrREL, &CPU::BMI, 2};
	// BNE
	m_jumpTable[0xD0] = {"BNE", &CPU::addrREL, &CPU::BNE, 2};
	// BPL
	m_jumpTable[0x10] = {"BPL", &CPU::addrREL, &CPU::BPL, 2};
	// BRK
	//m_jumpTable[0x00] = {"BRK", &CPU::addrIMP, &CPU::BRK, 7};
	// BVC
	m_jumpTable[0x50] = {"BVC", &CPU::addrREL, &CPU::BVC, 2};
	// BVS
	m_jumpTable[0x70] = {"BVS", &CPU::addrREL, &CPU::BVS, 2};
	//CLC
	//CLD
	//CLI
	//CLV
	//CMP
	//CPX
	//CPY
	//DEC
	//DEX
	//DEY
	//EOR
	m_jumpTable[0x41] = {"EOR", &CPU::addrINX, &CPU::EOR, 6};
	m_jumpTable[0x45] = {"EOR", &CPU::addrZPI, &CPU::EOR, 3};
	m_jumpTable[0x49] = {"EOR", &CPU::addrIMM, &CPU::EOR, 2};
	m_jumpTable[0x4D] = {"EOR", &CPU::addrABS, &CPU::EOR, 4};
	m_jumpTable[0x51] = {"EOR", &CPU::addrINY, &CPU::EOR, 5};
	m_jumpTable[0x55] = {"EOR", &CPU::addrZPX, &CPU::EOR, 4};
	m_jumpTable[0x59] = {"EOR", &CPU::addrABY, &CPU::EOR, 4};
	m_jumpTable[0x5D] = {"EOR", &CPU::addrABX, &CPU::EOR, 4};
	//INC
	//INX
	//INY
	//JMP
	//JSR
	//LDA
	//LDX
	//LDY
	//LSR
	m_jumpTable[0x4A] = {"LSR", &CPU::addrACC, &CPU::LSR, 2};
	m_jumpTable[0x46] = {"LSR", &CPU::addrZPI, &CPU::LSR, 5};
	m_jumpTable[0x4E] = {"LSR", &CPU::addrABS, &CPU::LSR, 6};
	m_jumpTable[0x56] = {"LSR", &CPU::addrZPX, &CPU::LSR, 6};
	m_jumpTable[0x5A] = {"LSR", &CPU::addrABX, &CPU::LSR, 7};
	//NOP
	//ORA
	m_jumpTable[0x01] = {"ORA", &CPU::addrINX, &CPU::ORA, 6};
	m_jumpTable[0x05] = {"ORA", &CPU::addrZPI, &CPU::ORA, 3};
	m_jumpTable[0x09] = {"ORA", &CPU::addrIMM, &CPU::ORA, 2};
	m_jumpTable[0x0D] = {"ORA", &CPU::addrABS, &CPU::ORA, 4};
	m_jumpTable[0x11] = {"ORA", &CPU::addrINY, &CPU::ORA, 5};
	m_jumpTable[0x15] = {"ORA", &CPU::addrZPX, &CPU::ORA, 4};
	m_jumpTable[0x19] = {"ORA", &CPU::addrABY, &CPU::ORA, 4};
	m_jumpTable[0x1D] = {"ORA", &CPU::addrABX, &CPU::ORA, 4};
	//PHA
	m_jumpTable[0x48] = {"PHA", &CPU::addrIMP, &CPU::PHA, 3};
	//PHP
	m_jumpTable[0x08] = {"PHP", &CPU::addrIMP, &CPU::PHP, 3};
	//PLA
	m_jumpTable[0x68] = {"PLA", &CPU::addrIMP, &CPU::PLA, 4};
	//PLP
	m_jumpTable[0x28] = {"PLP", &CPU::addrIMP, &CPU::PLP, 4};
	//ROL
	m_jumpTable[0x2A] = {"ROL", &CPU::addrACC, &CPU::ROL, 2};
	m_jumpTable[0x26] = {"ROL", &CPU::addrZPI, &CPU::ROL, 5};
	m_jumpTable[0x2E] = {"ROL", &CPU::addrABS, &CPU::ROL, 6};
	m_jumpTable[0x36] = {"ROL", &CPU::addrZPX, &CPU::ROL, 6};
	m_jumpTable[0x3A] = {"ROL", &CPU::addrABX, &CPU::ROL, 7};
	//ROR
	m_jumpTable[0x6A] = {"ROR", &CPU::addrACC, &CPU::ROR, 2};
	m_jumpTable[0x66] = {"ROR", &CPU::addrZPI, &CPU::ROR, 5};
	m_jumpTable[0x6E] = {"ROR", &CPU::addrABS, &CPU::ROR, 6};
	m_jumpTable[0x76] = {"ROR", &CPU::addrZPX, &CPU::ROR, 6};
	m_jumpTable[0x7A] = {"ROR", &CPU::addrABX, &CPU::ROR, 7};
	//RTI
	//RTS
	//SBC
	//SEC
	//SED
	//SEI
	//STA
	//STX
	//STY
	//TAX
	m_jumpTable[0xAA] = {"TAX", &CPU::addrIMP, &CPU::TAX, 2};
	//TAY
	m_jumpTable[0xA8] = {"TAY", &CPU::addrIMP, &CPU::TAY, 2};
	//TSX
	m_jumpTable[0xBA] = {"TSX", &CPU::addrIMP, &CPU::TSX, 2};
	//TXA
	m_jumpTable[0x8A] = {"TXA", &CPU::addrIMP, &CPU::TXA, 2};
	//TXS
	m_jumpTable[0x9A] = {"TXS", &CPU::addrIMP, &CPU::TXS, 2};
	//TYA
	m_jumpTable[0x98] = {"TYA", &CPU::addrIMP, &CPU::TYA, 2};

	Reset();
}

void CPU::Step()
{
	// resetting temps
	m_oopsCycles = 0;
	m_canOops = false;
	m_discard = 0;

	if (m_cycles == 0)
	{
		m_opcode = readByte();

		m_currentInstr = m_jumpTable[m_opcode];

		m_cycles = m_currentInstr.cycles;
		u16 addr = (this->*m_currentInstr.addrMode)();
		// execute instruction
		(this->*m_currentInstr.exec)(addr);

		if (m_canOops)
		{
			m_cycles += m_oopsCycles;
		}
		
	}

	m_cycles--;
}

void CPU::Reset()
{
	m_A = 0;
	m_X = 0;
	m_Y = 0;
	m_S = 0xFD;
	m_P = 0x34;

	// TODO: NOT YET
	// 6502 reads memory at $FFFC
	//u16 addr = 0xFFFC;
	//u16 lo = readMemory(addr);
	//u16 hi = readMemory(addr + 1);

	//m_PC = MAKE_WORD(hi, lo);

	// reset utility varibles
	m_canOops = false;
	m_oopsCycles = 0;
	m_discard = 0;
	m_cycles = 8;
}


u8 CPU::readMemory(u16 addr) const
{
	return m_bus->Read(addr);
}

u8 CPU::readByte()
{
	u8 value = readMemory(m_PC);
	m_PC++;
	return value;
}


void CPU::writeMemory(u16 addr, u8 val) const
{
	m_bus->Write(addr, val);
}

u16 CPU::addrIMP()
{
	return 0;
}

u16 CPU::addrIMM()
{
	u16 addr = m_PC++;
	return addr;
}

u16 CPU::addrZPI()
{
	u16 addr = readByte();
	addr &= 0x00FF;

	return addr;
}

u16 CPU::addrZPX()
{
	u16 addr = (readByte() + m_X);
	addr &= 0x00FF;
	
	return addr;
}

u16 CPU::addrZPY()
{
	u16 addr = (readByte() + m_Y);
	addr &= 0x00FF;

	return addr;
}


u16 CPU::addrABS()
{
	u16 lo = readByte();
	u16 hi = readByte();
	u16 addr = MAKE_WORD(hi, lo);

	return addr;
}

u16 CPU::addrABX()
{
	u16 lo = readByte();
	u16 hi = readByte();

	u16 addr = MAKE_WORD(hi, lo);
	addr += m_X;

	if ((addr & 0xFF00) != (hi << 8))
		m_oopsCycles++;
	
	return addr;
}

u16 CPU::addrABY()
{
	u16 lo = readByte();
	u16 hi = readByte();

	u16 addr = MAKE_WORD(hi, lo);
	addr += m_Y;

	if ((addr & 0xFF00) != (hi << 8))
	{
		m_oopsCycles++;
	}

	return addr;
}

u16 CPU::addrIND()
{
	u16 lo = readByte();
	u16 hi = readByte();

	u16 notyet = MAKE_WORD(hi, lo);

	u16 addr;

	// hardware bug
	if (lo == 0x00FF) 
	{
		addr = MAKE_WORD(readMemory(notyet & 0x00FF), readMemory(notyet));
	}
	else
	{
		addr = MAKE_WORD(readMemory(notyet + 1), readMemory(notyet));
	}

	return addr;
}

u16 CPU::addrINX()
{
	u16 base = readByte() + m_X;

	u16 lo = readMemory((base & 0x00FF));
	u16 hi = readMemory(((base + 1) & 0x00FF));


	u16 addr = MAKE_WORD(hi, lo);

	return addr;

}

u16 CPU::addrINY()
{
	u16 base = readByte();

	u16 lo = readMemory(base % 256);
	u16 hi = readMemory((base + 1) % 256);

	u16 addr = MAKE_WORD(hi, lo);
	addr += m_Y;

	if ((addr & 0xFF00) != (hi << 8))
	{
		m_oopsCycles++;
	}
	
	return addr;
}

u16 CPU::addrREL()
{
	u16 offset = readByte();

	// this contraption here is to emulate wrap around
	// could also be done by casting to s8, but I have grown attached
	// to the machine that killed Shinzo Abe as represented here
	// if bit 7 is set it will twos complement it to the negatives, but being u16

	if (offset & 0x80) {
		offset |= 0xff00;
	}

	u16 addr = m_PC + offset;

	return addr;
}


u16 CPU::addrACC()
{
	return 0;
}

/*
 * Instruction Add with carry
 * m_A = m_A + M + C
 * flags: C, V, N, Z
 * The overflow thing explained with a truth table
 * the overflow will be set when a and m share the same sign but r is different
 * A^R will be true when a and r have different symbol
 * ~(A^M) will be true when a and m have the same symbol
 * anding the two condition will get the result we want
 * ┌---------------------------------------┐
 * | A M R | V | A^R |~(A^M) | A^R &~(A^M) |
 * ├-------┼---┼-----┼-------┼-------------┤
 * | 0 0 0 | 0 |  0  |   1   |      0      |
 * | 0 0 1 | 1 |  1  |   1   |      1      |
 * | 0 1 0 | 0 |  0  |   0   |      0      |
 * | 0 1 1 | 0 |  1  |   0   |      0      |
 * | 1 0 0 | 0 |  1  |   0   |      0      |
 * | 1 0 1 | 0 |  0  |   0   |      0      |
 * | 1 1 0 | 1 |  1  |   1   |      1      |
 * | 1 1 1 | 0 |  0  |   1   |      0      |
 * └-------┴---┴-----┴-------┴-------------┘
 */
void CPU::ADC(u16 addr)
{
	// hack to get overflow
	u16 m = readMemory(addr);
	u16 a16 = m_A;
	u16 temp = m + a16;
	if (CHECK_FLAG(P_C_FLAG)) 
	{
		temp += 1;
	}
	// if overflow in bit 7 we set carry flag
	setFlagIf(P_C_FLAG, temp > 255);
	setFlagIf(P_Z_FLAG, (temp & 0x00FF) == 0);
	// Here should go something with decimal mode, but is not used, so whatever
	setFlagIf(P_V_FLAG, (~(a16 ^ m) & (a16 ^ temp)) & 0x0080);
	setFlagIf(P_N_FLAG, temp & 0x80);

	m_A = temp & 0x00FF;
	m_canOops = true;
}

/*
 * Instruction AND
 * m_A = m_A & M
 * Flags: Z, N
 */
void CPU::AND(u16 addr)
{
	u8 m = readMemory(addr);
	m_A = m_A & m;
	setFlagIf(P_Z_FLAG, m_A == 0);
	setFlagIf(P_N_FLAG, CHECK_BIT(m_A, 7));

	m_canOops = true;
	
}


/*
 * Instruction Arithemtic Shift Left
 * m_A = m_A * 2 | M = M * 2
 * Flags: Z, N, C
 */
void CPU::ASL(u16 addr)
{
	u8 m;
	if (isImplied())
	{
		m = m_A;
	}
	else
	{
		m = readMemory(addr);
	}

	setFlagIf(P_C_FLAG, m & 0x80);
	m <<= 1;
	m &= 0x00FF;
	setFlagIf(P_Z_FLAG, m == 0);
	setFlagIf(P_N_FLAG, m & 0x80);

	if (isImplied()) 
	{
		m_A = m;
	}
	else 
	{
		writeMemory(addr, m);
	}
}

/*
 * Instruction Branch if carry clear
 * if C == 0 --> PC = addr
 */
void CPU::BCC(u16 addr)
{
	branchIfCond(addr, !CHECK_FLAG(P_C_FLAG));
}
/*
 * Instruction Branch if carry set
 * if C == 1 --> PC = addr
 */
void CPU::BCS(u16 addr)
{
	branchIfCond(addr, CHECK_FLAG(P_C_FLAG));
}
/*
 * Instruction Branch if equal
 * if Z == 1 --> PC = addr
 */
void CPU::BEQ(u16 addr)
{
	branchIfCond(addr, CHECK_FLAG(P_Z_FLAG));
}

/*
 * Instruction Bit Test
 * A = A & M
 * Flags: N = M7, V = M6
 */
void CPU::BIT(u16 addr)
{
	u8 m = readMemory(addr);

	m_discard = m_A & m;

	setFlagIf(P_Z_FLAG, m_discard == 0);
	setFlagIf(P_V_FLAG, m & 0x40);
	setFlagIf(P_N_FLAG, m & 0x80);
}

/*
 * Instruction Branch if minus
 * if N == 1 --> PC = addr
 */
void CPU::BMI(u16 addr)
{
	branchIfCond(addr, CHECK_FLAG(P_N_FLAG));
}

/*
 * Instruction Branch if not equals
 * if Z == 0 --> PC = addr
 */
void CPU::BNE(u16 addr)
{
	branchIfCond(addr, CHECK_FLAG(P_Z_FLAG) == 0);
}

/*
 * Instruction Branch if positive
 * if N == 0 --> PC = addr
 */
void CPU::BPL(u16 addr)
{
	branchIfCond(addr, CHECK_FLAG(P_N_FLAG) == 0);

}

/*
 * Instruction Branch if overflow clear
 * if V == 0 --> PC = addr
 */
void CPU::BVC(u16 addr)
{
	branchIfCond(addr, CHECK_FLAG(P_V_FLAG) == 0);
}

/*
 * Instruction Branch if overflow set
 * if V == 1 --> PC = addr
 */
void CPU::BVS(u16 addr)
{
	branchIfCond(addr, CHECK_FLAG(P_V_FLAG));
}

/*
 * Instruction Exclusive or
 * m_A = m_A ^ M
 * Flags: Z, N
 */
void CPU::EOR(u16 addr)
{
	u8 m = readMemory(addr);

	m_A ^= m;

	setFlagIf(P_Z_FLAG, m_A == 0);
	setFlagIf(P_N_FLAG, m_A & 0x80);

	m_canOops = true;
}

/*
 * Instruction Logical Shift Right
 * A = A/2 | M = M/2
 * flags: C, Z, N
 */
void CPU::LSR(u16 addr)
{
	u8 m;
	if (isImplied())
	{
		m = m_A;
	}
	else
	{
		m = readMemory(addr);
	}

	setFlagIf(P_C_FLAG, m & 1);
	m >>= 1;
	m &= 0x00FF;
	setFlagIf(P_Z_FLAG, m == 0);
	setFlagIf(P_N_FLAG, m & 0x80);

	if (isImplied())
	{
		m_A = m;
	}
	else
	{
		writeMemory(addr, m);
	}
}

/*
 * Instruction Inclusive or
 * m_A = m_A | M
 * Flags: Z, N
 */
void CPU::ORA(u16 addr)
{
	u8 m = readMemory(addr);

	m_A |= m;

	setFlagIf(P_Z_FLAG, m_A == 0);
	setFlagIf(P_N_FLAG, m_A & 0x80);

	m_canOops = true;
}

/*
 * Instruction Push Accumulator
 * Pushes accumulator to the stack
 *
 * The stack is hardcoded in page $01 ($0100-$01FF)
 *
 * Important, The 6502 uses a reverse stack
 */
void CPU::PHA(u16 addr)
{
	stackPush(m_A);
}

/*
 * Instruction Push Processor status
 * Pushes accumulator to the stack
 */
void CPU::PHP(u16 addr)
{
	stackPush(m_P);
}

/*
 * Instruction Pull Accumulator
 * Pulls accumulator from the stack
 * Uses flag N, Z for some reason
 */
void CPU::PLA(u16 addr)
{
	m_A = stackPop();
	setFlagIf(P_N_FLAG, m_A & 0x80);
	setFlagIf(P_Z_FLAG, m_A == 0);
}
/*
 * Instruction Pull Processor Status
 * Pulls Processor status from the stack
 */
void CPU::PLP(u16 addr)
{
	m_P = stackPop();
}


/*
 * Instruction Rotate Left
 * move each byte A|M one place to the left
 * bit 0 is filled with carry
 * carry is filled with bit 7
 * flags: C, Z, N
 */
void CPU::ROL(u16 addr)
{
	u16 m;
	if (isImplied())
	{
		m = m_A;
	}
	else
	{
		m = readMemory(addr);
	}
	// we are in 16 bit realm so if we have
	// 0000 0000 1011 10001
	// and we rotate left we should end with
	// 0000 0001 0111 00010
	// anything above bit 7 will be cut in 8 bits
	// but in 16 we can store it to check flags
	m <<= 1;
	if (CHECK_FLAG(P_C_FLAG))
	{
		m |= 1;
	}

	setFlagIf(P_C_FLAG, m > 0xff);
	m &= 0x00FF;
	setFlagIf(P_N_FLAG, m & 0x80);
	setFlagIf(P_Z_FLAG, m == 0);

	if (isImplied())
	{
		m_A = m;
	}
	else
	{
		writeMemory(addr, m);
	}
}

/*
 * Instruction Rotate Right
 * move each byte A|M one place to the right
 * bit 7 is filled with carry
 * carry is filled with bit 0
 * flags: C, Z, N
 */
void CPU::ROR(u16 addr)
{
	u16 m;
	if (isImplied())
	{
		m = m_A;
	}
	else
	{
		m = readMemory(addr);
	}

	// we set bit 8 in case of carry so we can 
	// store it after shifting
	if (CHECK_FLAG(P_C_FLAG))
	{
		m |= 0x100;
	}
	setFlagIf(P_C_FLAG, m & 1);
	m >>= 1;

	m &= 0x00FF;
	setFlagIf(P_N_FLAG, m & 0x80);
	setFlagIf(P_Z_FLAG, m == 0);

	if (isImplied())
	{
		m_A = m;
	}
	else
	{
		writeMemory(addr, m);
	}
}
/*
 * Instruction Transfer Accumulator to X
 * X = A
 * flags: Z, N
 */
void CPU::TAX(u16 addr)
{
	transferRegTo(m_A, m_X);
}

/*
 * Instruction Transfer Accumulator to Y
 * Y = A
 * flags: Z, N
 */
void CPU::TAY(u16 addr)
{
	transferRegTo(m_A, m_Y);
}

/*
 * Instruction Transfer Stack pointer to X
 * X = S
 * flags: Z, N
 */
void CPU::TSX(u16 addr)
{
	transferRegTo(m_S, m_X);
}

/*
 * Instruction Transfer X to Accumulator
 * A = X
 * flags: Z, N
 */
void CPU::TXA(u16 addr)
{
	transferRegTo(m_X, m_A);
}

/*
 * Instruction Transfer X to Stack pointer
 * S = X
 * flags: Z, N
 */
void CPU::TXS(u16 addr) 
{
	transferRegTo(m_X, m_S);
}

/*
 * Instruction Transfer Y to Accumulator
 * A = Y
 * flags: Z, N
 */
void CPU::TYA(u16 addr) 
{ 
	transferRegTo(m_Y, m_A);
}

[[noreturn]]
void CPU::___(u16 addr) 
{
	// commit sudoku
	throw std::exception("Illegal instruction");
}

void CPU::setFlagIf(u8 flag, bool cond)
{
	if (cond) 
	{
		m_P |= (flag); 
	}
	else 
	{
		m_P &= ~(flag);
	}
}

bool CPU::isImplied() const
{
	return m_currentInstr.addrMode == &CPU::addrIMP || m_currentInstr.addrMode == &CPU::addrACC;
}

void CPU::branchIfCond(u16 addr, bool cond)
{
	// flag is clear
	if (cond)
	{
		m_cycles++;

		u8 page = (m_PC & 0xff00);
		
		m_PC = addr;
		// extra cycle for page change
		if (page != (m_PC & 0xff00))
		{
			m_cycles++;
		}
	}
}

void CPU::transferRegTo(u8 from, u8& to)
{
	to = from;
	setFlagIf(P_Z_FLAG, to == 0);
	setFlagIf(P_N_FLAG, to & 0x80);
}

void CPU::stackPush(u8 val)
{
	writeMemory(m_stackVectorBase + m_S, val);
	if (m_S == 0)
	{
		m_S = 0xFF;
	}
	else
	{
		m_S--;
	}
}
u8 CPU::stackPop()
{
	if (m_S == 0xFF)
	{
		m_S = 0;
	}
	else
	{
		m_S++;
	}
	return readMemory(m_stackVectorBase + m_S);
}

}

