#include "pch.hpp"

#include "CPU.hpp"

#include "Bus.hpp"

namespace Emu 
{

constexpr auto MAKE_WORD = [](const u16 hi, const u16 lo) -> u16 
{ 
	return static_cast<u16>((hi << 8)) | lo; 
};

CPU::CPU()
{
	// filling the jump table
	constexpr Instruction invalid = {
		"___",
		&CPU::addrIMM, 
		&CPU::XXX, 
		0
	};
	std::ranges::fill(m_jumpTable, invalid);
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
	m_jumpTable[0x00] = {"BRK", &CPU::addrIMP, &CPU::BRK, 7};
	// BVC
	m_jumpTable[0x50] = {"BVC", &CPU::addrREL, &CPU::BVC, 2};
	// BVS
	m_jumpTable[0x70] = {"BVS", &CPU::addrREL, &CPU::BVS, 2};
	//CLC
	m_jumpTable[0x18] = {"CLC", &CPU::addrIMP, &CPU::CLC, 2};
	//CLD
	m_jumpTable[0xD8] = {"CLD", &CPU::addrIMP, &CPU::CLD, 2};
	//CLI
	m_jumpTable[0x58] = {"CLI", &CPU::addrIMP, &CPU::CLI, 2};
	//CLV
	m_jumpTable[0xB8] = {"CLV", &CPU::addrIMP, &CPU::CLV, 2};
	//CMP
	m_jumpTable[0xC9] = {"CMP", &CPU::addrIMM, &CPU::CMP, 2};
	m_jumpTable[0xC5] = {"CMP", &CPU::addrZPI, &CPU::CMP, 3};
	m_jumpTable[0xD5] = {"CMP", &CPU::addrZPX, &CPU::CMP, 4};
	m_jumpTable[0xCD] = {"CMP", &CPU::addrABS, &CPU::CMP, 4};
	m_jumpTable[0xDD] = {"CMP", &CPU::addrABX, &CPU::CMP, 4};
	m_jumpTable[0xD9] = {"CMP", &CPU::addrABY, &CPU::CMP, 4};
	m_jumpTable[0xC1] = {"CMP", &CPU::addrINX, &CPU::CMP, 6};
	m_jumpTable[0xD1] = {"CMP", &CPU::addrINY, &CPU::CMP, 5};
	//CPX
	m_jumpTable[0xE0] = {"CPX", &CPU::addrIMM, &CPU::CPX, 2};
	m_jumpTable[0xE4] = {"CPX", &CPU::addrZPI, &CPU::CPX, 3};
	m_jumpTable[0xEC] = {"CPX", &CPU::addrABS, &CPU::CPX, 4};
	//CPY
	m_jumpTable[0xC0] = {"CPY", &CPU::addrIMM, &CPU::CPY, 2};
	m_jumpTable[0xC4] = {"CPY", &CPU::addrZPI, &CPU::CPY, 3};
	m_jumpTable[0xCC] = {"CPY", &CPU::addrABS, &CPU::CPY, 4};
	//DEC
	m_jumpTable[0xC6] = {"DEC", &CPU::addrZPI, &CPU::DEC, 5};
	m_jumpTable[0xD6] = {"DEC", &CPU::addrZPX, &CPU::DEC, 6};
	m_jumpTable[0xCE] = {"DEC", &CPU::addrABS, &CPU::DEC, 6};
	m_jumpTable[0xDE] = {"DEC", &CPU::addrABX, &CPU::DEC, 7};
	//DEX
	m_jumpTable[0xCA] = {"DEX", &CPU::addrIMP, &CPU::DEX, 2};
	//DEY
	m_jumpTable[0x88] = {"DEY", &CPU::addrIMP, &CPU::DEY, 2};
	//EOR
	m_jumpTable[0x49] = {"EOR", &CPU::addrIMM, &CPU::EOR, 2};
	m_jumpTable[0x45] = {"EOR", &CPU::addrZPI, &CPU::EOR, 3};
	m_jumpTable[0x55] = {"EOR", &CPU::addrZPX, &CPU::EOR, 4};
	m_jumpTable[0x4D] = {"EOR", &CPU::addrABS, &CPU::EOR, 4};
	m_jumpTable[0x5D] = {"EOR", &CPU::addrABX, &CPU::EOR, 4};
	m_jumpTable[0x59] = {"EOR", &CPU::addrABY, &CPU::EOR, 4};
	m_jumpTable[0x41] = {"EOR", &CPU::addrINX, &CPU::EOR, 6};
	m_jumpTable[0x51] = {"EOR", &CPU::addrINY, &CPU::EOR, 5};
	//INC
	m_jumpTable[0xE6] = {"INC", &CPU::addrZPI, &CPU::INC, 5};
	m_jumpTable[0xF6] = {"INC", &CPU::addrZPX, &CPU::INC, 6};
	m_jumpTable[0xEE] = {"INC", &CPU::addrABS, &CPU::INC, 6};
	m_jumpTable[0xFE] = {"INC", &CPU::addrABX, &CPU::INC, 7};
	//INX
	m_jumpTable[0xE8] = {"INX", &CPU::addrIMP, &CPU::INX, 2};
	//INY
	m_jumpTable[0xC8] = {"INY", &CPU::addrIMP, &CPU::INY, 2};
	//JMP
	m_jumpTable[0x4C] = {"JMP", &CPU::addrABS, &CPU::JMP, 3};
	m_jumpTable[0x6C] = {"JMP", &CPU::addrIND, &CPU::JMP, 5};
	//JSR
	m_jumpTable[0x20] = {"JSR", &CPU::addrABS, &CPU::JSR, 6};
	//LDA
	m_jumpTable[0xA9] = {"LDA", &CPU::addrIMM, &CPU::LDA, 2};
	m_jumpTable[0xA5] = {"LDA", &CPU::addrZPI, &CPU::LDA, 3};
	m_jumpTable[0xB5] = {"LDA", &CPU::addrZPX, &CPU::LDA, 4};
	m_jumpTable[0xAD] = {"LDA", &CPU::addrABS, &CPU::LDA, 4};
	m_jumpTable[0xBD] = {"LDA", &CPU::addrABX, &CPU::LDA, 4};
	m_jumpTable[0xB9] = {"LDA", &CPU::addrABY, &CPU::LDA, 4};
	m_jumpTable[0xA1] = {"LDA", &CPU::addrINX, &CPU::LDA, 6};
	m_jumpTable[0xB1] = {"LDA", &CPU::addrINY, &CPU::LDA, 5};
	//LDX
	m_jumpTable[0xA2] = {"LDX", &CPU::addrIMM, &CPU::LDX, 2};
	m_jumpTable[0xA6] = {"LDX", &CPU::addrZPI, &CPU::LDX, 3};
	m_jumpTable[0xB6] = {"LDX", &CPU::addrZPY, &CPU::LDX, 4};
	m_jumpTable[0xAE] = {"LDX", &CPU::addrABS, &CPU::LDX, 4};
	m_jumpTable[0xBE] = {"LDX", &CPU::addrABY, &CPU::LDX, 4};
	//LDY
	m_jumpTable[0xA0] = {"LDY", &CPU::addrIMM, &CPU::LDY, 2};
	m_jumpTable[0xA4] = {"LDY", &CPU::addrZPI, &CPU::LDY, 3};
	m_jumpTable[0xB4] = {"LDY", &CPU::addrZPX, &CPU::LDY, 4};
	m_jumpTable[0xAC] = {"LDY", &CPU::addrABS, &CPU::LDY, 4};
	m_jumpTable[0xBC] = {"LDY", &CPU::addrABX, &CPU::LDY, 4};
	//LSR
	m_jumpTable[0x4A] = {"LSR", &CPU::addrACC, &CPU::LSR, 2};
	m_jumpTable[0x46] = {"LSR", &CPU::addrZPI, &CPU::LSR, 5};
	m_jumpTable[0x4E] = {"LSR", &CPU::addrABS, &CPU::LSR, 6};
	m_jumpTable[0x56] = {"LSR", &CPU::addrZPX, &CPU::LSR, 6};
	m_jumpTable[0x5A] = {"LSR", &CPU::addrABX, &CPU::LSR, 7};
	//NOP
	m_jumpTable[0xEA] = {"NOP", &CPU::addrIMP, &CPU::NOP, 2};
	//ORA
	m_jumpTable[0x09] = {"ORA", &CPU::addrIMM, &CPU::ORA, 2};
	m_jumpTable[0x05] = {"ORA", &CPU::addrZPI, &CPU::ORA, 3};
	m_jumpTable[0x15] = {"ORA", &CPU::addrZPX, &CPU::ORA, 4};
	m_jumpTable[0x0D] = {"ORA", &CPU::addrABS, &CPU::ORA, 4};
	m_jumpTable[0x1D] = {"ORA", &CPU::addrABX, &CPU::ORA, 4};
	m_jumpTable[0x19] = {"ORA", &CPU::addrABY, &CPU::ORA, 4};
	m_jumpTable[0x01] = {"ORA", &CPU::addrINX, &CPU::ORA, 6};
	m_jumpTable[0x11] = {"ORA", &CPU::addrINY, &CPU::ORA, 5};
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
	m_jumpTable[0x36] = {"ROL", &CPU::addrZPX, &CPU::ROL, 6};
	m_jumpTable[0x2E] = {"ROL", &CPU::addrABS, &CPU::ROL, 6};
	m_jumpTable[0x3A] = {"ROL", &CPU::addrABX, &CPU::ROL, 7};
	//ROR
	m_jumpTable[0x6A] = {"ROR", &CPU::addrACC, &CPU::ROR, 2};
	m_jumpTable[0x66] = {"ROR", &CPU::addrZPI, &CPU::ROR, 5};
	m_jumpTable[0x76] = {"ROR", &CPU::addrZPX, &CPU::ROR, 6};
	m_jumpTable[0x6E] = {"ROR", &CPU::addrABS, &CPU::ROR, 6};
	m_jumpTable[0x7A] = {"ROR", &CPU::addrABX, &CPU::ROR, 7};
	// RTI
	m_jumpTable[0x40] = {"RTI", &CPU::addrIMP, &CPU::RTI, 6};
	//RTS
	m_jumpTable[0x60] = {"RTS", &CPU::addrIMP, &CPU::RTS, 6};
	//SBC
	m_jumpTable[0xE9] = {"SBC", &CPU::addrIMM, &CPU::SBC, 2};
	m_jumpTable[0xE5] = {"SBC", &CPU::addrZPI, &CPU::SBC, 3};
	m_jumpTable[0xF5] = {"SBC", &CPU::addrZPX, &CPU::SBC, 4};
	m_jumpTable[0xED] = {"SBC", &CPU::addrABS, &CPU::SBC, 4};
	m_jumpTable[0xFD] = {"SBC", &CPU::addrABX, &CPU::SBC, 4};
	m_jumpTable[0xF9] = {"SBC", &CPU::addrABY, &CPU::SBC, 4};
	m_jumpTable[0xE1] = {"SBC", &CPU::addrINX, &CPU::SBC, 6};
	m_jumpTable[0xF1] = {"SBC", &CPU::addrINY, &CPU::SBC, 5};
	//SEC
	m_jumpTable[0x38] = {"SEC", &CPU::addrIMP, &CPU::SEC, 2};
	//SED
	m_jumpTable[0xF8] = {"SED", &CPU::addrIMP, &CPU::SED, 2};
	//SEI
	m_jumpTable[0x78] = {"SEI", &CPU::addrIMP, &CPU::SEI, 2};
	//STA
	m_jumpTable[0x85] = {"STA", &CPU::addrZPI, &CPU::STA, 3};
	m_jumpTable[0x95] = {"STA", &CPU::addrZPX, &CPU::STA, 4};
	m_jumpTable[0x8D] = {"STA", &CPU::addrABS, &CPU::STA, 4};
	m_jumpTable[0x9D] = {"STA", &CPU::addrABX, &CPU::STA, 4};
	m_jumpTable[0x89] = {"STA", &CPU::addrABY, &CPU::STA, 5};
	m_jumpTable[0x81] = {"STA", &CPU::addrINX, &CPU::STA, 6};
	m_jumpTable[0x91] = {"STA", &CPU::addrINY, &CPU::STA, 6};
	//STX
	m_jumpTable[0x86] = {"STX", &CPU::addrZPI, &CPU::STX, 3};
	m_jumpTable[0x96] = {"STX", &CPU::addrZPY, &CPU::STX, 4};
	m_jumpTable[0x8E] = {"STX", &CPU::addrABS, &CPU::STX, 4};
	//STY
	m_jumpTable[0x84] = {"STY", &CPU::addrZPI, &CPU::STY, 3};
	m_jumpTable[0x94] = {"STY", &CPU::addrZPX, &CPU::STY, 4};
	m_jumpTable[0x8C] = {"STY", &CPU::addrABS, &CPU::STY, 4};
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
		// get address
		u16 addr = (this->*m_currentInstr.addrMode)();
		// execute instruction
		(this->*m_currentInstr.exec)(addr);

		if (m_canOops)
		{
			m_cycles += m_oopsCycles;
		}
		
	}
	m_totalCycles++;
	m_cycles--;
}

void CPU::Reset()
{
	m_A = 0;
	m_X = 0;
	m_Y = 0;
	m_S = 0xFD;
	m_P = 0x34;

	// 6502 reads memory at $FFFC/D
	u16 lo = memoryRead(m_resetVectorL);
	u16 hi = memoryRead(m_resetVectorH);

	m_PC = MAKE_WORD(hi, lo);

	// reset utility varibles
	m_canOops = false;
	m_oopsCycles = 0;
	m_discard = 0;
	// this takes 8 cycles
	m_cycles = 8;
}

void CPU::IRQ()
{
	if (checkFlag(P_I_FLAG)) 
		return;

	const u8 pcL = m_PC & 0x00ff;
	const u8 pcH = (m_PC & 0xff00) >> 8;

	stackPush(pcH);
	stackPush(pcL);

	clearFlag(P_B_FLAG);

	stackPush(m_P);

	setFlag(P_I_FLAG);
	const u8 irqL = memoryRead(m_irqVectorL);
	const u8 irqH = memoryRead(m_irqVectorH);

	m_PC = MAKE_WORD(irqH, irqL);
}

void CPU::NMI()
{
	const u8 pcL = m_PC & 0x00ff;
	const u8 pcH = (m_PC & 0xff00) >> 8;

	stackPush(pcH);
	stackPush(pcL);

	clearFlag(P_B_FLAG);

	stackPush(m_P);

	setFlag(P_I_FLAG);

	const u8 nmiL = memoryRead(m_nmiVectorL);
	const u8 nmiH = memoryRead(m_nmiVectorH);

	m_PC = MAKE_WORD(nmiH, nmiL);
}


u8 CPU::memoryRead(const u16 addr) const
{
#ifdef NES_EMU_DEBUG
	if (m_bus == nullptr) 
		std::throw_with_nested(std::runtime_error("Cpu was not linked to a bus"));
#endif
	return m_bus->Read(addr);
}

void CPU::memoryWrite(const u16 addr, const u8 val) const
{
#ifdef NES_EMU_DEBUG
	if (m_bus == nullptr) 
		std::throw_with_nested(std::runtime_error("Cpu was not linked to a bus"));
#endif
	m_bus->Write(addr, val);
}


u8 CPU::readByte()
{
	const u8 value = memoryRead(m_PC);
	m_PC++;
	return value;
}



u16 CPU::addrIMP()
{
	return 0;
}

u16 CPU::addrIMM()
{
	const u16 addr = m_PC++;
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
	const u8 lo = readByte();
	const u8 hi = readByte();
	const u16 addr = MAKE_WORD(hi, lo);

	return addr;
}

u16 CPU::addrABX()
{
	const u16 lo = readByte();
	const u16 hi = readByte();

	u16 addr = MAKE_WORD(hi, lo);
	addr += m_X;

	if ((addr & 0xFF00) != (hi << 8))
		m_oopsCycles++;
	
	return addr;
}

u16 CPU::addrABY()
{
	const u16 lo = readByte();
	const u16 hi = readByte();

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
	const u16 lo = readByte();
	const u16 hi = readByte();

	const u16 notyet = MAKE_WORD(hi, lo);

	u16 addr;

	// hardware bug
	if (lo == 0x00FF) 
	{
		addr = MAKE_WORD(memoryRead(notyet & 0x00FF), memoryRead(notyet));
	}
	else
	{
		addr = MAKE_WORD(memoryRead(notyet + 1), memoryRead(notyet));
	}

	return addr;
}

u16 CPU::addrINX()
{
	const u16 base = readByte() + m_X;

	const u16 lo = memoryRead((base & 0x00FF));
	const u16 hi = memoryRead(((base + 1) & 0x00FF));


	const u16 addr = MAKE_WORD(hi, lo);

	return addr;

}

u16 CPU::addrINY()
{
	const u16 base = readByte();

	const u16 lo = memoryRead(base & 0x00ff);
	const u16 hi = memoryRead((base + 1) & 0x00ff);

	const u16 addr = MAKE_WORD(hi, lo) + m_Y;

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

	const u16 addr = m_PC + offset;

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
 * 
 * The overflow thing explained with a truth table
 * http://www.6502.org/tutorials/vflag.html#2.4
 * 
 * the overflow will be set when a and m share the same sign but r is different
 * A^R will be true when a and r have different symbol
 * M^R will be true when m and r have different symbol
 * anding the two condition will get the result we want
 * then we extract the sign anding with 0x80
 * ┌-------┬---┬-----┬-----┬-----------┐
 * | A M R | V | A^R | M^R | A^R & A^M |
 * ├-------┼---┼-----┼-----┼-----------┤
 * | 0 0 0 | 0 |  0  |  0  |     0     |
 * | 0 0 1 | 1 |  1  |  1  |     1     |
 * | 0 1 0 | 0 |  0  |  1  |     0     |
 * | 0 1 1 | 0 |  1  |  0  |     0     |
 * | 1 0 0 | 0 |  1  |  0  |     0     |
 * | 1 0 1 | 0 |  0  |  1  |     0     |
 * | 1 1 0 | 1 |  1  |  1  |     1     |
 * | 1 1 1 | 0 |  0  |  0  |     0     |
 * └-------┴---┴-----┴-----┴-----------┘
 */
void CPU::ADC(const u16 addr)
{
	// hack to get overflow
	const u16 m = memoryRead(addr);
	const u16 a = m_A;
	u16 r = m + a;
	if (checkFlag(P_C_FLAG)) 
	{
		r += 1;
	}
	// if overflow in bit 7 we set carry flag
	setFlagIf(P_C_FLAG, r > 255);
	setFlagIf(P_Z_FLAG, (r & 0x00FF) == 0);
	// Here should go something with decimal mode, but is not used, so whatever
	setFlagIf(P_V_FLAG, (a ^ r) & (m ^ r) & 0x0080);
	setFlagIf(P_N_FLAG, r & 0x80);

	m_A = r & 0x00FF;
	m_canOops = true;
}

/*
 * Instruction AND
 * m_A = m_A & M
 * Flags: Z, N
 */
void CPU::AND(const u16 addr)
{
	const u8 m = memoryRead(addr);
	m_A &= m;
	setFlagIf(P_Z_FLAG, m_A == 0);
	setFlagIf(P_N_FLAG, m_A & 0x80);

	m_canOops = true;
}


/*
 * Instruction Arithemtic Shift Left
 * m_A = m_A * 2 | M = M * 2
 * Flags: Z, N, C
 */
void CPU::ASL(const u16 addr)
{
	u8 m;
	if (isImplied())
	{
		m = m_A;
	}
	else
	{
		m = memoryRead(addr);
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
		memoryWrite(addr, m);
	}
}

/*
 * Instruction Branch if carry clear
 * if C == 0 --> PC = addr
 */
void CPU::BCC(const u16 addr)
{
	branchIfCond(addr, !checkFlag(P_C_FLAG));
}
/*
 * Instruction Branch if carry set
 * if C == 1 --> PC = addr
 */
void CPU::BCS(const u16 addr)
{
	branchIfCond(addr, checkFlag(P_C_FLAG));
}
/*
 * Instruction Branch if equal
 * if Z == 1 --> PC = addr
 */
void CPU::BEQ(const u16 addr)
{
	branchIfCond(addr, checkFlag(P_Z_FLAG));
}

/*
 * Instruction Bit Test
 * A = A & M
 * Flags: N = M7, V = M6
 */
void CPU::BIT(const u16 addr)
{
	const u8 m = memoryRead(addr);

	m_discard = m_A & m;

	setFlagIf(P_Z_FLAG, m_discard == 0);
	setFlagIf(P_V_FLAG, m & 0x40);
	setFlagIf(P_N_FLAG, m & 0x80);
}

/*
 * Instruction Branch if minus
 * if N == 1 --> PC = addr
 */
void CPU::BMI(const u16 addr)
{
	branchIfCond(addr, checkFlag(P_N_FLAG));
}

/*
 * Instruction Branch if not equals
 * if Z == 0 --> PC = addr
 */
void CPU::BNE(const u16 addr)
{
	branchIfCond(addr, !checkFlag(P_Z_FLAG));
}

/*
 * Instruction Branch if positive
 * if N == 0 --> PC = addr
 */
void CPU::BPL(const u16 addr)
{
	branchIfCond(addr, !checkFlag(P_N_FLAG));

}
/*
 * Instruction Force Interrupt
 * Pushes PC and P to the stack
 * Then loads the IRQ Interrupt vector ($FFFE/F) 
 * into the PC and sets the B flag;
 * 
 * This instruction is 7 cycles:
 * 1 read opcode
 * 2 read padding byte
 * 3 pushing PC hi byte
 * 4 pushing PC lo byte
 * 5 pushing P with B flag
 * 6 fetch low byte of irq vector from $FFFE
 * 7 fetch hi byte of irq vector from $FFFF
 */
void CPU::BRK(const u16 addr)
{
	// padding byte since BRK is a 2 byte instruction
	// even tho it uses implied mode
	m_PC++;
	// automatically setted when an interrupt is triggered
	const u8 pclo = m_PC & 0x00ff;
	const u8 pchi = (m_PC & 0xff00) >> 8;

	stackPush(pchi);
	stackPush(pclo);

	stackPush(m_P | P_1_FLAG | P_B_FLAG);
	setFlag(P_I_FLAG);

	const u8 lo = memoryRead(m_irqVectorL);
	const u8 hi = memoryRead(m_irqVectorH);

	m_PC = MAKE_WORD(hi, lo);
}

/*
 * Instruction Branch if overflow clear
 * if V == 0 --> PC = addr
 */
void CPU::BVC(const u16 addr)
{
	branchIfCond(addr, !checkFlag(P_V_FLAG));
}

/*
 * Instruction Branch if overflow set
 * if V == 1 --> PC = addr
 */
void CPU::BVS(const u16 addr)
{
	branchIfCond(addr, checkFlag(P_V_FLAG));
}
/*
 * Instruction Clear Carry Flag
 */
void CPU::CLC(const u16 addr)
{
	m_P &= ~P_C_FLAG;
}

/*
 * Instruction Clear Decimal Mode
 */
void CPU::CLD(const u16 addr)
{
	m_P &= ~P_D_FLAG;
}

/*
 * Instruction Clear Interrupt Disable
 */
void CPU::CLI(const u16 addr)
{
	m_P &= ~P_I_FLAG;
}

/*
 * Instruction Clear Overflow Flag
 */
void CPU::CLV(const u16 addr)
{
	m_P &= ~P_V_FLAG;
}

/*
 * Instruction Compare
 * A - M
 * result is discarded
 * flags: Z, C, N
 */
void CPU::CMP(const u16 addr)
{
	const u8 m = memoryRead(addr);

	m_discard = m_A - m;

	setFlagIf(P_C_FLAG, m_A >= m);
	setFlagIf(P_Z_FLAG, m_A == m);
	setFlagIf(P_N_FLAG, m_discard & 0x80);

	m_canOops = true;
}

/*
 * Instruction Compare X Register
 * X - M
 * result is discarded
 * flags: Z, C, N
 */
void CPU::CPX(const u16 addr)
{
	const u8 m = memoryRead(addr);

	m_discard = m_X - m;

	setFlagIf(P_C_FLAG, m_X >= m);
	setFlagIf(P_Z_FLAG, m_X == m);
	setFlagIf(P_N_FLAG, m_discard & 0x80);
}

/*
 * Instruction Compare Y Register
 * Y - M
 * result is discarded
 * flags: Z, C, N
 */
void CPU::CPY(const u16 addr)
{
	const u8 m = memoryRead(addr);

	m_discard = m_Y - m;

	setFlagIf(P_C_FLAG, m_Y >= m);
	setFlagIf(P_Z_FLAG, m_Y == m);
	setFlagIf(P_N_FLAG, m_discard & 0x80);
}

/*
 * Instruction Decrement Memory
 * M = M - 1
 * flags: N, Z
 */
void CPU::DEC(const u16 addr)
{
	u8 m = memoryRead(addr);

	m = (m - 1) & 0x00ff;

	setFlagIf(P_N_FLAG, m & 0x80);
	setFlagIf(P_Z_FLAG, m == 0);

	memoryWrite(addr, m);
}

/*
 * Instruction Decrement X Register
 * X = X - 1
 * flags: N, Z
 */
void CPU::DEX(const u16 addr)
{
	m_X = (m_X - 1) & 0x00ff;

	setFlagIf(P_N_FLAG, m_X & 0x80);
	setFlagIf(P_Z_FLAG, m_X == 0);
}

/*
 * Instruction Decrement Y Register
 * Y = Y - 1
 * flags: N, Z
 */
void CPU::DEY(const u16 addr)
{
	m_Y = (m_Y - 1) & 0x00ff;

	setFlagIf(P_N_FLAG, m_Y & 0x80);
	setFlagIf(P_Z_FLAG, m_Y == 0);
}

/*
 * Instruction Exclusive or
 * m_A = m_A ^ M
 * Flags: Z, N
 */
void CPU::EOR(const u16 addr)
{
	const u8 m = memoryRead(addr);

	m_A ^= m;

	setFlagIf(P_Z_FLAG, m_A == 0);
	setFlagIf(P_N_FLAG, m_A & 0x80);

	m_canOops = true;
}

/*
 * Instruction Increment Memory
 * M = M + 1 (wrpas if m = 0xff)
 * flags: Z, N
 */
void CPU::INC(const u16 addr)
{
	u8 m = memoryRead(addr);

	m = (m + 1) & 0x00ff;

	setFlagIf(P_N_FLAG, m & 0x80);
	setFlagIf(P_Z_FLAG, m == 0);

	memoryWrite(addr, m);
}

/*
 * Instruction Increment X Register
 * X = X + 1 (wrpas if X = 0xff)
 * flags: Z, N
 */
void CPU::INX(const u16 addr)
{
	m_X = (m_X + 1) & 0x00ff;

	setFlagIf(P_N_FLAG, m_X & 0x80);
	setFlagIf(P_Z_FLAG, m_X == 0);
}

/*
 * Instruction Increment Y Register
 * Y = Y + 1 (wraps if Y = 0xff)
 * flags: Z, N
 */
void CPU::INY(const u16 addr)
{
	m_Y = (m_Y + 1) & 0x00ff;

	setFlagIf(P_N_FLAG, m_Y & 0x80);
	setFlagIf(P_Z_FLAG, m_Y == 0);
}

/*
 * Instruction Jump
 * PC = M
 */
void CPU::JMP(const u16 addr)
{
	m_PC = addr;
}

/*
 * Instruction Jump from Subroutine
 * Pushes pc - 1 to the stack
 * and sets the pc to M
 */
void CPU::JSR(const u16 addr)
{
	const u16 temp = m_PC - 1;
	const u8 pcL = temp & 0x00ff;
	const u8 pcH = (temp & 0xff00) >> 8;

	stackPush(pcH);
	stackPush(pcL);

	m_PC = addr;
}

/*
 * Instruction Load Accumulator
 * A = M
 * flags: Z, N
 */
void CPU::LDA(const u16 addr)
{
	const u8 m = memoryRead(addr);

	transferRegTo(m, m_A);

	m_canOops = true;
}

/*
 * Instruction Load X
 * X = M
 * flags: Z, N
 */
void CPU::LDX(const u16 addr)
{
	const u8 m = memoryRead(addr);

	transferRegTo(m, m_X);

	m_canOops = true;
}

/*
 * Instruction Load Y
 * X = M
 * flags: Z, N
 */
void CPU::LDY(const u16 addr)
{
	const u8 m = memoryRead(addr);

	transferRegTo(m, m_Y);

	m_canOops = true;
}

/*
 * Instruction Logical Shift Right
 * A = A/2 | M = M/2
 * flags: C, Z, N
 */
void CPU::LSR(const u16 addr)
{
	u8 m;
	if (isImplied())
	{
		m = m_A;
	}
	else
	{
		m = memoryRead(addr);
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
		memoryWrite(addr, m);
	}
}

/*
 * Instruction No Operation
 * Does nothing
 */
void CPU::NOP(const u16 addr)
{
}

/*
 * Instruction Inclusive or
 * m_A = m_A | M
 * Flags: Z, N
 */
void CPU::ORA(const u16 addr)
{
	const u8 m = memoryRead(addr);

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
void CPU::PHA(const u16 addr)
{
	stackPush(m_A);
}

/*
 * Instruction Push Processor status
 * Pushes accumulator to the stack
 * 
 * also pushes B flag
 */
void CPU::PHP(const u16 addr)
{
	stackPush(m_P | P_B_FLAG | P_1_FLAG);
}

/*
 * Instruction Pull Accumulator
 * Pulls accumulator from the stack
 * Uses flag N, Z for some reason
 */
void CPU::PLA(const u16 addr)
{
	m_A = stackPop();
	setFlagIf(P_N_FLAG, m_A & 0x80);
	setFlagIf(P_Z_FLAG, m_A == 0);
}
/*
 * Instruction Pull Processor Status
 * Pulls Processor status from the stack
 */
void CPU::PLP(const u16 addr)
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
void CPU::ROL(const u16 addr)
{
	u16 m;
	if (isImplied())
	{
		m = m_A;
	}
	else
	{
		m = memoryRead(addr);
	}
	// we are in 16 bit realm so if we have
	// 0000 0000 1011 10001
	// and we rotate left we should end with
	// 0000 0001 0111 00010
	// anything above bit 7 will be cut in 8 bits
	// but in 16 we can store it to check flags
	m <<= 1;
	if (checkFlag(P_C_FLAG))
	{
		m |= 1;
	}

	setFlagIf(P_C_FLAG, m > 0xff);
	m &= 0x00FF;
	setFlagIf(P_N_FLAG, m & 0x80);
	setFlagIf(P_Z_FLAG, m == 0);

	if (isImplied())
	{
		m_A = static_cast<u8>(m);
	}
	else
	{
		memoryWrite(addr, static_cast<u8>(m));
	}
}

/*
 * Instruction Rotate Right
 * move each byte A|M one place to the right
 * bit 7 is filled with carry
 * carry is filled with bit 0
 * flags: C, Z, N
 */
void CPU::ROR(const u16 addr)
{
	u16 m;
	if (isImplied())
	{
		m = m_A;
	}
	else
	{
		m = memoryRead(addr);
	}

	// we set bit 8 in case of carry so we can 
	// store it after shifting
	if (checkFlag(P_C_FLAG))
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
		m_A = static_cast<u8>(m);
	}
	else
	{
		memoryWrite(addr, static_cast<u8>(m));
	}
}
/*
 * Instruction Return from Interrupt
 * Used at the end of an interrupt processing routine
 * Works oppostie of BRK, pulls P and PC from the stack
 * 
 * Also discard B flag
 */
void CPU::RTI(u16 addr)
{
	m_P = stackPop();

	const u8 pclo = stackPop();
	const u8 pchi = stackPop();

	m_PC = MAKE_WORD(pchi, pclo);
}

/*
 * Instruction Return from Subroutine
 * pulls the PC minus one from the stack
 */
void CPU::RTS(u16 addr)
{

	const u8 lo = stackPop();
	const u8 hi = stackPop();

	m_PC = MAKE_WORD(hi, lo) + 1;
}

/*
 * Instruction Substract with Carry
 * Substracts the not of the carry flag for some reason
 * A = A - M - (1 - C)
 * flags: Z, C, V, N
 *
 * The overflow thing explained with a truth table
 * http://www.6502.org/tutorials/vflag.html#2.4
 * 
 * the overflow will be set when a and m share the same sign but r is different
 * A^R will be true when a and r have different symbol
 * M^R will be true when m and r have different symbol
 * anding the two condition will get the result we want
 * then we extract the sign anding with 0x80
 * ┌-------┬---┬-----┬-----┬-----------┐
 * | A M R | V | A^R | M^R | A^R & A^M |
 * ├-------┼---┼-----┼-----┼-----------┤
 * | 0 0 0 | 0 |  0  |  0  |     0     |
 * | 0 0 1 | 1 |  1  |  1  |     1     |
 * | 0 1 0 | 0 |  0  |  1  |     0     |
 * | 0 1 1 | 0 |  1  |  0  |     0     |
 * | 1 0 0 | 0 |  1  |  0  |     0     |
 * | 1 0 1 | 0 |  0  |  1  |     0     |
 * | 1 1 0 | 1 |  1  |  1  |     1     |
 * | 1 1 1 | 0 |  0  |  0  |     0     |
 * └-------┴---┴-----┴-----┴-----------┘
 */
void CPU::SBC(const u16 addr)
{
	const u16 m = memoryRead(addr);
	const u16 a = m_A;

	u16 r = a - m;
	if (!checkFlag(P_C_FLAG))
	{
		r = r - 1;
	}
	
	setFlagIf(P_N_FLAG, r & 0x80);
	setFlagIf(P_Z_FLAG, (r & 0x00ff) == 0);
	setFlagIf(P_V_FLAG, (a ^ r) & (m ^ r) & 0x0080);
	// some decimal code should go there but in the nes it is disabled
	setFlagIf(P_C_FLAG, r & 0x00ff);

	m_A = r & 0x00ff;

	m_canOops = true;
}

/*
 * Instruction Set Carry Flag
 */
void CPU::SEC(u16 addr)
{
	m_P |= P_C_FLAG;
}

/*
 * Instruction Set Decimal Mode
 */
void CPU::SED(u16 addr)
{
	m_P |= P_D_FLAG;
}

/*
 * Instruction Set Insterrupt Disable
 */
void CPU::SEI(u16 addr)
{
	m_P |= P_I_FLAG;
}

/*
 * Instruction Store Accumulator
 * M = A
 */
void CPU::STA(const u16 addr)
{
	memoryWrite(addr, m_A);
}

/*
 * Instruction Store Accumulator
 * M = X
 */
void CPU::STX(const u16 addr)
{
	memoryWrite(addr, m_X);
}

/*
 * Instruction Store Accumulator
 * M = Y
 */
void CPU::STY(const u16 addr)
{
	memoryWrite(addr, m_Y);

}

/*
 * Instruction Transfer Accumulator to X
 * X = A
 * flags: Z, N
 */
void CPU::TAX(const u16 addr)
{
	transferRegTo(m_A, m_X);
}

/*
 * Instruction Transfer Accumulator to Y
 * Y = A
 * flags: Z, N
 */
void CPU::TAY(const u16 addr)
{
	transferRegTo(m_A, m_Y);
}

/*
 * Instruction Transfer Stack pointer to X
 * X = S
 * flags: Z, N
 */
void CPU::TSX(const u16 addr)
{
	transferRegTo(m_S, m_X);
}

/*
 * Instruction Transfer X to Accumulator
 * A = X
 * flags: Z, N
 */
void CPU::TXA(const u16 addr)
{
	transferRegTo(m_X, m_A);
}

/*
 * Instruction Transfer X to Stack pointer
 * S = X
 */
void CPU::TXS(const u16 addr) 
{
	m_S = m_X;
}

/*
 * Instruction Transfer Y to Accumulator
 * A = Y
 * flags: Z, N
 */
void CPU::TYA(const u16 addr) 
{ 
	transferRegTo(m_Y, m_A);
}

[[noreturn]]
void CPU::XXX(const u16 addr) 
{
	// commit sudoku
	std::throw_with_nested(std::runtime_error("Illegal instruction"));
}

void CPU::setFlagIf(const u8 flag, const bool cond) noexcept
{
	if (cond) 
	{
		setFlag(flag);
	}
	else 
	{
		clearFlag(flag);
	}
}

void CPU::setFlag(const u8 flag) noexcept
{
	m_P |= flag;
}

void CPU::clearFlag(const u8 flag) noexcept
{
	m_P &= ~flag;
}

bool CPU::checkFlag(const u8 flag) const noexcept
{
	return m_P & flag ? true : false;
}

bool CPU::isImplied() const noexcept
{
	return m_currentInstr.addrMode == &CPU::addrIMP || m_currentInstr.addrMode == &CPU::addrACC;
}

void CPU::branchIfCond(const u16 addr, bool cond) noexcept
{
	// flag is clear
	if (cond)
	{
		m_cycles++;

		const u8 page = static_cast<u8>(m_PC & 0xff00);
		
		m_PC = addr;
		// extra cycle for page change
		if (page != (m_PC & 0xff00))
		{
			m_cycles++;
		}
	}
}

void CPU::transferRegTo(const u8 from, u8& to) noexcept
{
	to = from;
	setFlagIf(P_Z_FLAG, to == 0);
	setFlagIf(P_N_FLAG, to & 0x80);
}

void CPU::stackPush(const u8 val) noexcept
{
	memoryWrite(m_stackVector + m_S, val);
	if (m_S == 0)
	{
		m_S = 0xFF;
	}
	else
	{
		m_S--;
	}
}
u8 CPU::stackPop() noexcept
{
	if (m_S == 0xFF)
	{
		m_S = 0;
	}
	else
	{
		m_S++;
	}
	return memoryRead(m_stackVector + m_S);
}

}