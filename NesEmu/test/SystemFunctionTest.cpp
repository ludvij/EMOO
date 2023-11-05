#include "pch.hpp"

#include "TestConfig.hpp"

class TestSystemFunction : public TestFixture {};


TEST_F(TestSystemFunction, NOP)
{
	asse.Assemble(R"(
		nop
		nop
		nop
		nop
	)");

	const Emu::u8 a = cpu.A();
	const Emu::u8 x = cpu.X();
	const Emu::u8 y = cpu.Y();
	const Emu::u8 s = cpu.S();
	const Emu::u8 p = cpu.P();
	const Emu::u16 pc = cpu.PC();

	clearCycles(2);
	ASSERT_EQ(a, cpu.A());
	ASSERT_EQ(x, cpu.X());
	ASSERT_EQ(y, cpu.Y());
	ASSERT_EQ(s, cpu.S());
	ASSERT_EQ(p, cpu.P());
	ASSERT_EQ(pc + 1, cpu.PC());

	clearCycles(2);
	ASSERT_EQ(a, cpu.A());
	ASSERT_EQ(x, cpu.X());
	ASSERT_EQ(y, cpu.Y());
	ASSERT_EQ(s, cpu.S());
	ASSERT_EQ(p, cpu.P());
	ASSERT_EQ(pc + 2, cpu.PC());

	clearCycles(2);
	ASSERT_EQ(a, cpu.A());
	ASSERT_EQ(x, cpu.X());
	ASSERT_EQ(y, cpu.Y());
	ASSERT_EQ(s, cpu.S());
	ASSERT_EQ(p, cpu.P());
	ASSERT_EQ(pc + 3, cpu.PC());

	clearCycles(2);
	ASSERT_EQ(a, cpu.A());
	ASSERT_EQ(x, cpu.X());
	ASSERT_EQ(y, cpu.Y());
	ASSERT_EQ(s, cpu.S());
	ASSERT_EQ(p, cpu.P());
	ASSERT_EQ(pc + 4, cpu.PC());
}

TEST_F(TestSystemFunction, BRK)
{
	asse.Assemble(R"(
		brk
	)");

	const Emu::u16 pc = cpu.PC();
	const Emu::u8 p = cpu.P();

	clearCycles(7);

	ASSERT_EQ(p | Emu::P_B_FLAG | Emu::P_1_FLAG, memory.Read(0x01fb));
	const Emu::u8 lo = memory.Read(0x01fc);
	const Emu::u8 hi = memory.Read(0x01fd);
	ASSERT_EQ(pc+2, static_cast<Emu::u16>(hi << 8) | lo);
}

TEST_F(TestSystemFunction, RTI)
{
	asse.Assemble(R"(
		brk
		rti
	)");

	const Emu::u16 pc = cpu.PC();
	const Emu::u8 p = cpu.P();

	clearCycles(7);
	// imaginary interrupt
	cpu.SetPC(2);
	clearCycles(6);

	ASSERT_EQ(p | Emu::P_B_FLAG | Emu::P_1_FLAG, cpu.P());
	ASSERT_EQ(pc+2, cpu.PC());
}