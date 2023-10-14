#include "pch.hpp"

#include "CPU.hpp"

#include "Bus.hpp"

#define MAKE_WORD(hi, lo) (((hi) << 8) | (lo))

#define CHECK_BIT(val, bit) (((val) >> (bit)) & 1)

#define CHECK_FLAG(flag) ((m_P & (flag)) > 0)
#define SET_FLAG_IF(flag, cond) if (cond) m_P |= (flag); else m_P &= ~(flag)

namespace Emu {


CPU::CPU()
{
	fillJumpTable();

}

void CPU::Step()
{
	if (m_cycles == 0)
	{
		m_opcode = readByte();

		m_cycles = m_jumpTable[m_opcode].cycles;
		m_addr = m_jumpTable[m_opcode].addrMode();
		m_jumpTable[m_opcode].exec();

		if (m_canOops)
		{
			m_cycles += m_oopsCycles;
		}
		m_oopsCycles = 0;
		m_canOops = false;
	}

	m_cycles--;
}

void CPU::fillJumpTable()
{
	// filling the jump table
	Instruction invalid = {
		&addrIMM, 
		&___, 
		0
	};
	std::fill_n(m_jumpTable, 256, invalid);
	// ADC
	m_jumpTable[0x69] = {&addrIMM, &ADC, 2};
	m_jumpTable[0x65] = {&addrZPI, &ADC, 3};
	m_jumpTable[0x75] = {&addrZPX, &ADC, 4};
	m_jumpTable[0x6D] = {&addrABS, &ADC, 4};
	m_jumpTable[0x7D] = {&addrABX, &ADC, 4};
	m_jumpTable[0x79] = {&addrABY, &ADC, 4};
	m_jumpTable[0x61] = {&addrINX, &ADC, 6};
	m_jumpTable[0x71] = {&addrINY, &ADC, 5};
	// AND
	m_jumpTable[0x29] = {&addrIMM, &AND, 2};
	m_jumpTable[0x25] = {&addrZPI, &AND, 3};
	m_jumpTable[0x35] = {&addrZPX, &AND, 4};
	m_jumpTable[0x2D] = {&addrABS, &AND, 4};
	m_jumpTable[0x3D] = {&addrABX, &AND, 4};
	m_jumpTable[0x39] = {&addrABY, &AND, 4};
	m_jumpTable[0x21] = {&addrINX, &AND, 6};
	m_jumpTable[0x31] = {&addrINY, &AND, 5};
	// ASL
	m_jumpTable[0x0A] = {&addrACC, &ASL, 2};
	m_jumpTable[0x06] = {&addrZPI, &ASL, 5};
	m_jumpTable[0x16] = {&addrZPX, &ASL, 6};
	m_jumpTable[0x0E] = {&addrABS, &ASL, 6};
	m_jumpTable[0x1A] = {&addrABX, &ASL, 7};

}

u8 CPU::read(u16 addr) const
{
	return m_bus->Read(addr);
}

u8 CPU::readByte()
{
	u8 value = m_bus->Read(m_PC);
	m_PC++;
	return value;
}


void CPU::write(u16 addr, u8 val) const
{
	m_bus->Write(addr, val);
}

u8 CPU::fetch()
{
	// workaround for accumulator m
	if (m_jumpTable[m_opcode].addrMode == &addrACC)
		return m_A;
	return read(m_addr);
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
		addr = MAKE_WORD(read(notyet & 0x00FF), read(notyet));
	}
	else
	{
		addr = MAKE_WORD(read(notyet + 1), read(notyet));
	}

	return addr;
}

u16 CPU::addrINX()
{
	u16 base = readByte() + m_X;

	u16 lo = read((base & 0x00FF));
	u16 hi = read(((base + 1) & 0x00FF));


	u16 addr = MAKE_WORD(hi, lo);

	return addr;

}

u16 CPU::addrINY()
{
	u16 base = readByte();

	u16 lo = read(base % 256);
	u16 hi = read((base + 1) % 256);

	u16 addr = MAKE_WORD(hi, lo);
	addr += m_Y;

	if ((addr & 0xFF00) != (hi << 8))
	{
		m_oopsCycles++;
	}
	
	return addr;
}

// we need an 8-bit signed offset relative to pc
u16 CPU::addrREL()
{
	u16 offset = readByte();

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
* A = A + M + C
* flags: C, V, N, Z
*/
void CPU::ADC()
{
	// hack to get overflow
	u16 m = fetch();
	u16 a16 = m_A;
	u16 temp = m + a16;
	if (CHECK_FLAG(P_C_FLAG)) 
	{
		temp += 1;
	}
	// if overflow in bit 7 we set carry flag
	SET_FLAG_IF(P_C_FLAG, temp > 255);
	SET_FLAG_IF(P_Z_FLAG, (temp & 0x00FF) == 0);
	// Here should go something with decimal mode, but is not used, so whatever
	SET_FLAG_IF(P_V_FLAG, (~(a16 ^ m) & (a16 ^ temp)) & 0x0080);
	SET_FLAG_IF(P_N_FLAG, temp & 0x80);

	m_A = temp & 0x00FF;
	m_canOops = true;
}
/*
* Instruction AND
* A = A & M
* Flags: Z, N
*/
void CPU::AND()
{
	u8 m = fetch();
	m_A = m_A & m;
	SET_FLAG_IF(P_Z_FLAG, m_A == 0);
	SET_FLAG_IF(P_N_FLAG, CHECK_BIT(m_A, 7));

	m_canOops = true;
}


/*
 * Instruction Arithemtic Shift Left
 * A = A * 2 | M = M * 2
 * Flags: Z, N, C
 */
void CPU::ASL()
{
	u8 m = fetch();

	SET_FLAG_IF(P_C_FLAG, m & 0x80);
	m <<= 1;
	m &= 0x00FF;
	SET_FLAG_IF(P_Z_FLAG, m == 0);
	SET_FLAG_IF(P_N_FLAG, m & 0x80);
	if (m_jumpTable[m_opcode].addrMode == &addrACC) 
	{
		m_A = m;
	}
	else 
	{
		write(m_addr, m);
	}
}

}

