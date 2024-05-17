
#include "../TestConfig.hpp"


class TestStack : public TestFixture
{

};

TEST_F(TestStack, TSX)
{
	asse.Assemble(R"(
		tsx
		tsx
	)");

	console.GetCpu().SetS(0x80);

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().X(), 0x80);
	ASSERT_TRUE(console.GetCpu().P() & Emu::Flag::N);

	console.GetCpu().SetS(0);

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().X(), 0x00);
	ASSERT_TRUE(console.GetCpu().P() & Emu::Flag::Z);
}

TEST_F(TestStack, TXS)
{
	asse.Assemble(R"(
		txs
		txs
	)");

	console.GetCpu().SetX(0x80);

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().S(), 0x80);

	console.GetCpu().SetX(0);

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().S(), 0x00);
}

TEST_F(TestStack, STACK_PUSH)
{
	asse.Assemble(R"(
		pha
		php
		pha
		php
	)");
	Emu::u8 base = console.GetCpu().S();

	console.GetCpu().SetA(12);
	console.GetCpu().SetP(23);

	clearCycles(3);
	ASSERT_EQ(console.GetBus().Read(0x0100 + console.GetCpu().S() + 1), 12);
	ASSERT_EQ(console.GetCpu().S() + 1, base);
	clearCycles(3);
	ASSERT_EQ(console.GetBus().Read(0x0100 + console.GetCpu().S() + 1), 23 | Emu::Flag::B | Emu::Flag::U);
	ASSERT_EQ(console.GetCpu().S() + 2, base);
	clearCycles(3);
	ASSERT_EQ(console.GetBus().Read(0x0100 + console.GetCpu().S() + 1), 12);
	ASSERT_EQ(console.GetCpu().S() + 3, base);
	clearCycles(3);
	ASSERT_EQ(console.GetBus().Read(0x0100 + console.GetCpu().S() + 1), 23 | Emu::Flag::B | Emu::Flag::U);
	ASSERT_EQ(console.GetCpu().S() + 4, base);
}

TEST_F(TestStack, STACK_POP)
{
	asse.Assemble(R"(
        php
        php
		pha
		pha
		pla
		pla
		plp
		plp
	)");

	console.GetCpu().SetP(0x23);
	clearCycles(3);
	// 0x23, -
	console.GetCpu().SetP(12);
	clearCycles(3);
	// 12, 0x23, -
	console.GetCpu().SetA(0x80);
	clearCycles(3);
	// 0x80, 12, 0x23, -
	console.GetCpu().SetA(0);
	clearCycles(3);
	// 0, 0x80, 12, 0x23, -

	Emu::u8 base = console.GetCpu().S();
	clearCycles(4);
	// 0x80, 12, 0x23, -
	ASSERT_EQ(0, console.GetCpu().A());
	ASSERT_EQ(static_cast<Emu::u8>( console.GetCpu().S() - 1 ), base);
	ASSERT_TRUE(console.GetCpu().P() & Emu::Flag::Z);

	clearCycles(4);
	// 12, 0x23, -
	ASSERT_EQ(0x80, console.GetCpu().A());
	ASSERT_EQ(static_cast<Emu::u8>( console.GetCpu().S() - 2 ), base);
	ASSERT_TRUE(console.GetCpu().P() & Emu::Flag::N);


	clearCycles(4);
	// 0x23, -
	ASSERT_EQ(12 | Emu::Flag::U, console.GetCpu().P());
	ASSERT_EQ(static_cast<Emu::u8>( console.GetCpu().S() - 3 ), base);


	clearCycles(4);
	// -
	ASSERT_EQ(0x23 | Emu::Flag::U, console.GetCpu().P());
	ASSERT_EQ(static_cast<Emu::u8>( console.GetCpu().S() - 4 ), base);

}