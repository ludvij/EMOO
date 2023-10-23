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

	const Emu::u8 a = bus.GetCpu().A();
	const Emu::u8 x = bus.GetCpu().X();
	const Emu::u8 y = bus.GetCpu().Y();
	const Emu::u8 s = bus.GetCpu().S();
	const Emu::u8 p = bus.GetCpu().P();
	const Emu::u16 pc = bus.GetCpu().PC();

	clearCycles(2);
	ASSERT_EQ(a, bus.GetCpu().A());
	ASSERT_EQ(x, bus.GetCpu().X());
	ASSERT_EQ(y, bus.GetCpu().Y());
	ASSERT_EQ(s, bus.GetCpu().S());
	ASSERT_EQ(p, bus.GetCpu().P());
	ASSERT_EQ(pc + 1, bus.GetCpu().PC());

	clearCycles(2);
	ASSERT_EQ(a, bus.GetCpu().A());
	ASSERT_EQ(x, bus.GetCpu().X());
	ASSERT_EQ(y, bus.GetCpu().Y());
	ASSERT_EQ(s, bus.GetCpu().S());
	ASSERT_EQ(p, bus.GetCpu().P());
	ASSERT_EQ(pc + 2, bus.GetCpu().PC());

	clearCycles(2);
	ASSERT_EQ(a, bus.GetCpu().A());
	ASSERT_EQ(x, bus.GetCpu().X());
	ASSERT_EQ(y, bus.GetCpu().Y());
	ASSERT_EQ(s, bus.GetCpu().S());
	ASSERT_EQ(p, bus.GetCpu().P());
	ASSERT_EQ(pc + 3, bus.GetCpu().PC());

	clearCycles(2);
	ASSERT_EQ(a, bus.GetCpu().A());
	ASSERT_EQ(x, bus.GetCpu().X());
	ASSERT_EQ(y, bus.GetCpu().Y());
	ASSERT_EQ(s, bus.GetCpu().S());
	ASSERT_EQ(p, bus.GetCpu().P());
	ASSERT_EQ(pc + 4, bus.GetCpu().PC());
}

TEST_F(TestSystemFunction, BRK)
{
	asse.Assemble(R"(
		brk
	)");

	const Emu::u16 pc = bus.GetCpu().PC();
	const Emu::u8 p = bus.GetCpu().P();

	clearCycles(7);

	ASSERT_EQ(p | Emu::P_B_FLAG | Emu::P_1_FLAG, bus.Read(0x01fb));
	const Emu::u8 lo = bus.Read(0x01fc);
	const Emu::u8 hi = bus.Read(0x01fd);
	ASSERT_EQ(pc+2, static_cast<Emu::u16>(hi << 8) | lo);
}

TEST_F(TestSystemFunction, RTI)
{
	asse.Assemble(R"(
		brk
		rti
	)");

	const Emu::u16 pc = bus.GetCpu().PC();
	const Emu::u8 p = bus.GetCpu().P();

	clearCycles(7);
	// imaginary interrupt
	bus.GetCpu().SetPC(2);
	clearCycles(6);

	ASSERT_EQ(p | Emu::P_B_FLAG | Emu::P_1_FLAG, bus.GetCpu().P());
	ASSERT_EQ(pc+2, bus.GetCpu().PC());
}