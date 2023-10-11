#include "pch.hpp"

#include "CPU.hpp"

namespace Emu {


CPU::Instruction CPU::s_jumpTable[256];

CPU::CPU()
	: m_A(0)
	, m_X(0)
	, m_Y(0)
	, m_S(0xFD)
	, m_P(0x34)
{
	fillJumpTable();
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

}

