#include "pch.hpp"

#include "CPU.hpp"

#include "Bus.hpp"

#define MAKE_WORD(hi, lo) (((hi) << 8) | (lo))

#define CHECK_BIT(val, bit) (((val) >> (bit)) & 1)

#define CHECK_FLAG(flag) ((P & (flag)) > 0)
#define SET_FLAG_IF(flag, cond) if (cond) P |= (flag); else P &= ~(flag)

namespace Emu {


CPU::CPU()
{
	fillJumpTable();

}

void CPU::Step()
{
	if (cycles == 0)
	{
		opcode = readByte();

		currentInstr = &jumpTable[opcode];
		cycles = currentInstr->cycles;
		addr = currentInstr->addrMode();
		currentInstr->exec();

		if (canOops)
		{
			cycles += oopsCycles;
		}
		oopsCycles = 0;
		canOops = false;
	}

	cycles--;
}

void CPU::fillJumpTable()
{
	// filling the jump table
	Instruction invalid = {
		&addrIMM, 
		&___, 
		0
	};
	std::fill_n(jumpTable, 256, invalid);
	// ADC
	jumpTable[0x69] = {&addrIMM, &ADC, 2};
	jumpTable[0x65] = {&addrZPI, &ADC, 3};
	jumpTable[0x75] = {&addrZPX, &ADC, 4};
	jumpTable[0x6D] = {&addrABS, &ADC, 4};
	jumpTable[0x7D] = {&addrABX, &ADC, 4};
	jumpTable[0x79] = {&addrABY, &ADC, 4};
	jumpTable[0x61] = {&addrINX, &ADC, 6};
	jumpTable[0x71] = {&addrINY, &ADC, 5};
	// AND
	jumpTable[0x29] = {&addrIMM, &AND, 2};
	jumpTable[0x25] = {&addrZPI, &AND, 3};
	jumpTable[0x35] = {&addrZPX, &AND, 4};
	jumpTable[0x2D] = {&addrABS, &AND, 4};
	jumpTable[0x3D] = {&addrABX, &AND, 4};
	jumpTable[0x39] = {&addrABY, &AND, 4};
	jumpTable[0x21] = {&addrINX, &AND, 6};
	jumpTable[0x31] = {&addrINY, &AND, 5};
	// ASL
	jumpTable[0x0A] = {&addrACC, &ASL, 2};
	jumpTable[0x06] = {&addrZPI, &ASL, 5};
	jumpTable[0x16] = {&addrZPX, &ASL, 6};
	jumpTable[0x0E] = {&addrABS, &ASL, 6};
	jumpTable[0x1A] = {&addrABX, &ASL, 7};

}

u8 CPU::read(u16 addr) const
{
	return bus->Read(addr);
}

u8 CPU::readByte()
{
	u8 value = bus->Read(PC);
	PC++;
	return value;
}


void CPU::write(u16 addr, u8 val) const
{
	bus->Write(addr, val);
}

u8 CPU::fetch()
{
	// workaround for accumulator m
	if (jumpTable[opcode].addrMode == &addrACC)
		return A;
	return read(addr);
}


u16 CPU::addrIMP()
{
	return 0;
}

u16 CPU::addrIMM()
{
	u16 addr = PC++;
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
	u16 addr = (readByte() + X);
	addr &= 0x00FF;
	
	return addr;
}

u16 CPU::addrZPY()
{
	u16 addr = (readByte() + Y);
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
	addr += X;

	if ((addr & 0xFF00) != (hi << 8))
		oopsCycles++;
	
	return addr;
}

u16 CPU::addrABY()
{
	u16 lo = readByte();
	u16 hi = readByte();

	u16 addr = MAKE_WORD(hi, lo);
	addr += Y;

	if ((addr & 0xFF00) != (hi << 8))
	{
		oopsCycles++;
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
	u16 base = readByte() + X;

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
	addr += Y;

	if ((addr & 0xFF00) != (hi << 8))
	{
		oopsCycles++;
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

	u16 addr = PC + offset;

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
	u16 a16 = A;
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

	A = temp & 0x00FF;
	canOops = true;
}
/*
* Instruction AND
* A = A & M
* Flags: Z, N
*/
void CPU::AND()
{
	u8 m = fetch();
	A = A & m;
	SET_FLAG_IF(P_Z_FLAG, A == 0);
	SET_FLAG_IF(P_N_FLAG, CHECK_BIT(A, 7));

	canOops = true;
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
	if (jumpTable[opcode].addrMode == &addrACC) 
	{
		A = m;
	}
	else 
	{
		write(addr, m);
	}
}

}

