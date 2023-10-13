#include "pch.hpp"

#include "CPU.hpp"

#include "Bus.hpp"

namespace Emu {


CPU::Instruction CPU::s_jumpTable[256];

CPU::CPU()
{
	fillJumpTable();
}

void CPU::Step()
{
	if (m_cycles == 0)
	{
		Byte isntr = readByte();

		Byte cycles = s_jumpTable[isntr].cycles;
		Word addr = (*s_jumpTable[isntr].addrMode)();
		(*s_jumpTable[isntr].opcode)(addr);

		m_cycles += (m_oopsOp & m_oopsAddr);
	}

	m_cycles--;
}

void CPU::fillJumpTable() const
{
	// filling the jump table
	Instruction invalid = {
		nullptr, 
		nullptr, 
		0
	};
	std::fill_n(s_jumpTable, 256, invalid);
	//TODO: https://www.nesdev.org/wiki/CPU_unofficial_opcodes#Games_using_unofficial_opcodes
}

Byte CPU::read(Word addr) const
{
	return m_bus->Read(addr);
}

Byte CPU::readByte()
{
	Byte value = m_bus->Read(m_PC);
	m_PC++;
	return value;
}


void CPU::write(Word addr, Byte val) const
{
	m_bus->Write(addr, val);
}


Word CPU::addrIMP()
{
	return 0;
}

Word CPU::addrIMM()
{
	Word addr = m_PC++;
	return addr;
}

Word CPU::addrZPI()
{
	Word addr = readByte();
	addr &= 0x00ff;

	return addr;
}

Word CPU::addrZPIX()
{
	Word addr = (readByte() + m_X);
	addr &= 0x00ff;
	
	return addr;
}

Word CPU::addrZPIY()
{
	Word addr = (readByte() + m_Y);
	addr &= 0x00ff;

	return addr;
}


Word CPU::addrABS()
{
	Word lo = readByte();
	Word hi = readByte();
	Word addr = (hi << 8) | lo;

	return addr;
}

Word CPU::addrABSX()
{
	Word lo = readByte();
	Word hi = readByte();

	Word addr = (hi << 8) | lo;
	addr += m_X;

	if ((addr & 0xFF00) != (hi << 8))
		m_oopsAddr++;
	
	return addr;
}

Word CPU::addrABSY()
{
	Word lo = readByte();
	Word hi = readByte();

	Word addr = (hi << 8) | lo;
	addr += m_Y;

	if ((addr & 0xFF00) != (hi << 8))
		m_oopsAddr++;

	return addr;
}

Word CPU::addrIND()
{
	Word lo = readByte();
	Word hi = readByte();

	Word notyet = (hi << 8) | lo;

	Word addr;

	// hardware bug
	if (lo == 0x00FF)
		addr = (read(notyet & 0x00ff) << 8) | read(notyet);
	else
		addr = (read(notyet + 1) << 8) | read(notyet);

	return addr;
}

Word CPU::addrINDX()
{
	Word base = readByte() + m_X;

	Word lo = read((base & 0x00ff));
	Word hi = read(((base + 1) & 0x00FF));


	Word addr = (hi << 8) | lo;

	return addr;

}

Word CPU::addrINDY()
{
	Word base = readByte();

	Word lo = read(base % 256);
	Word hi = read((base + 1) % 256);
	Word addr = (hi << 8) | lo;
	addr += m_Y;

	if ((addr & 0xFF00) != (hi << 8))
		m_oopsAddr++;
	
	return addr;
}

// we need an 8-bit signed offset relative to pc
Word CPU::addrREL()
{
	Word offset = readByte();

	if (offset & 0x80)
		offset |= 0xff00;

	Word addr = m_PC + offset;

	return addr;
}


Word CPU::addrACC()
{
	return 0;
}


}

