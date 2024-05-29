#include "pch.hpp"
#include "A6502Core.hpp"

u8 A6502::GetBytesForAddressingMode(AddressingModeName am)
{
	typedef A6502::AddressingModeName A;

	switch (am)
	{
	case A::IMP: [[fallthrough]];
	case A::ACC: return 1;
	case A::IM2: [[fallthrough]];
	case A::IMM: [[fallthrough]];
	case A::ZPI: [[fallthrough]];
	case A::ZPX: [[fallthrough]];
	case A::ZPY: [[fallthrough]];
	case A::REL: return 2;
	case A::ABS: [[fallthrough]];
	case A::ABX: [[fallthrough]];
	case A::ABY: [[fallthrough]];
	case A::IND: [[fallthrough]];
	case A::INX: [[fallthrough]];
	case A::INY: return 3;
	default: std::unreachable();
	}
}

bool A6502::IsConditionalJump(Opcode op)
{
	typedef A6502::InstructionName I;
	return
		op.instruction == I::BCC || op.instruction == I::BCS ||
		op.instruction == I::BVC || op.instruction == I::BVS ||
		op.instruction == I::BMI || op.instruction == I::BPL ||
		op.instruction == I::BEQ || op.instruction == I::BNE;
}

bool A6502::IsUnconditionalJump(Opcode op)
{
	typedef A6502::InstructionName I;
	typedef A6502::AddressingModeName A;
	return
		// ignoring indirect jump
		//op.instruction == I::JMP && op.mode == A::IND ||
		op.instruction == I::JMP && op.mode == A::ABS ||
		op.instruction == I::JSR || op.instruction == I::RTS ||
		op.instruction == I::BRK || op.instruction == I::RTI;
}
