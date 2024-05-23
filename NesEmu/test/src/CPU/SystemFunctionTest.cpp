
#include "../TestConfig.hpp"

class TestSystemFunction : public TestFixture
{
};


TEST_F(TestSystemFunction, NOP)
{
	asse.Assemble(R"(
		nop
		nop
		nop
		nop
	)");

	const u8 a = console.GetCpu().A();
	const u8 x = console.GetCpu().X();
	const u8 y = console.GetCpu().Y();
	const u8 s = console.GetCpu().S();
	const u8 p = console.GetCpu().P();
	const u16 pc = console.GetCpu().PC();

	clearCycles(2);
	ASSERT_EQ(a, console.GetCpu().A());
	ASSERT_EQ(x, console.GetCpu().X());
	ASSERT_EQ(y, console.GetCpu().Y());
	ASSERT_EQ(s, console.GetCpu().S());
	ASSERT_EQ(p, console.GetCpu().P());
	ASSERT_EQ(pc + 1, console.GetCpu().PC());

	clearCycles(2);
	ASSERT_EQ(a, console.GetCpu().A());
	ASSERT_EQ(x, console.GetCpu().X());
	ASSERT_EQ(y, console.GetCpu().Y());
	ASSERT_EQ(s, console.GetCpu().S());
	ASSERT_EQ(p, console.GetCpu().P());
	ASSERT_EQ(pc + 2, console.GetCpu().PC());

	clearCycles(2);
	ASSERT_EQ(a, console.GetCpu().A());
	ASSERT_EQ(x, console.GetCpu().X());
	ASSERT_EQ(y, console.GetCpu().Y());
	ASSERT_EQ(s, console.GetCpu().S());
	ASSERT_EQ(p, console.GetCpu().P());
	ASSERT_EQ(pc + 3, console.GetCpu().PC());

	clearCycles(2);
	ASSERT_EQ(a, console.GetCpu().A());
	ASSERT_EQ(x, console.GetCpu().X());
	ASSERT_EQ(y, console.GetCpu().Y());
	ASSERT_EQ(s, console.GetCpu().S());
	ASSERT_EQ(p, console.GetCpu().P());
	ASSERT_EQ(pc + 4, console.GetCpu().PC());
}

TEST_F(TestSystemFunction, BRK)
{
	asse.Assemble(R"(
		brk
	)");

	const u16 pc = console.GetCpu().PC();
	const u8 p = console.GetCpu().P();

	clearCycles(7);

	ASSERT_EQ(p | Emu::ProcessorStatus::Flags::B | Emu::ProcessorStatus::Flags::U, console.GetBus().Read(0x01fb));
	const u8 lo = console.GetBus().Read(0x01fc);
	const u8 hi = console.GetBus().Read(0x01fd);
	ASSERT_EQ(pc + 2, static_cast<u16>( hi << 8 ) | lo);
}

TEST_F(TestSystemFunction, RTI)
{
	asse.Assemble(R"(
		brk
		rti
	)");

	const u16 pc = console.GetCpu().PC();
	const u8 p = console.GetCpu().P();

	clearCycles(7);
	// imaginary interrupt
	console.GetCpu().SetPC(2);
	clearCycles(6);

	ASSERT_EQ(p | Emu::ProcessorStatus::Flags::U, console.GetCpu().P());
	ASSERT_EQ(pc + 2, console.GetCpu().PC());
}