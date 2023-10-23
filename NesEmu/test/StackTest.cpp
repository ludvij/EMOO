#include "pch.hpp"

#include "TestConfig.hpp"


class TestStack : public TestFixture
{

};

TEST_F(TestStack, TSX)
{
	asse.Assemble(R"(
		tsx
		tsx
	)");

	bus.GetCpu().SetS(0x80);

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().X(), 0x80);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_N_FLAG);

	bus.GetCpu().SetS(0);

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().X(), 0x00);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_Z_FLAG);
}

TEST_F(TestStack, TXS)
{
	asse.Assemble(R"(
		txs
		txs
	)");

	bus.GetCpu().SetX(0x80);

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().S(), 0x80);

	bus.GetCpu().SetX(0);

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().S(), 0x00);
}

TEST_F(TestStack, STACK_PUSH)
{
	asse.Assemble(R"(
		pha
		php
		pha
		php
	)");
	Emu::u8 base = bus.GetCpu().S();

	bus.GetCpu().SetA(12);
	bus.GetCpu().SetP(23);

	clearCycles(3);
	ASSERT_EQ(bus.Read(0x0100 + bus.GetCpu().S() + 1), 12);
	ASSERT_EQ(bus.GetCpu().S() + 1, base);
	clearCycles(3);
	ASSERT_EQ(bus.Read(0x0100 + bus.GetCpu().S() + 1), 23 | Emu::P_B_FLAG | Emu::P_1_FLAG);
	ASSERT_EQ(bus.GetCpu().S() + 2, base);
	clearCycles(3);
	ASSERT_EQ(bus.Read(0x0100 + bus.GetCpu().S() + 1), 12);
	ASSERT_EQ(bus.GetCpu().S() + 3, base);
	clearCycles(3);
	ASSERT_EQ(bus.Read(0x0100 + bus.GetCpu().S() + 1), 23 | Emu::P_B_FLAG | Emu::P_1_FLAG);
	ASSERT_EQ(bus.GetCpu().S() + 4, base);
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

	bus.GetCpu().SetP(0x23);
	clearCycles(3);
	// 0x23, -
	bus.GetCpu().SetP(12);
	clearCycles(3);
	// 12, 0x23, -
	bus.GetCpu().SetA(0x80);
	clearCycles(3);
	// 0x80, 12, 0x23, -
	bus.GetCpu().SetA(0);
	clearCycles(3);
	// 0, 0x80, 12, 0x23, -

	Emu::u8 base = bus.GetCpu().S();
	clearCycles(4);
	// 0x80, 12, 0x23, -
	ASSERT_EQ(0, bus.GetCpu().A());
	ASSERT_EQ(static_cast<Emu::u8>(bus.GetCpu().S() - 1), base);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_Z_FLAG);

	clearCycles(4);
	// 12, 0x23, -
	ASSERT_EQ(0x80, bus.GetCpu().A());
	ASSERT_EQ(static_cast<Emu::u8>(bus.GetCpu().S() - 2), base);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_N_FLAG);


	clearCycles(4);
	// 0x23, -
	ASSERT_EQ(12 | Emu::P_1_FLAG | Emu::P_B_FLAG, bus.GetCpu().P());
	ASSERT_EQ(static_cast<Emu::u8>(bus.GetCpu().S() - 3), base);


	clearCycles(4);
	// -
	ASSERT_EQ(0x23 | Emu::P_1_FLAG | Emu::P_B_FLAG, bus.GetCpu().P());
	ASSERT_EQ(static_cast<Emu::u8>(bus.GetCpu().S() - 4), base);

}