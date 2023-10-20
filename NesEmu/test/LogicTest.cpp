#include "pch.hpp"

#include <gtest/gtest.h>


#include "TestConfig.hpp"


class TestLogic : public TestFixture
{

};


TEST_F(TestLogic, AND)
{
	asse.Assemble(R"(
		and #12
		and #$c0
	)");
	bus.GetCpu().SetA(0);
	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().A(), 0);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_Z_FLAG);


	bus.GetCpu().SetA(0x80);

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().A(), 0x80);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_N_FLAG);
	ASSERT_FALSE(bus.GetCpu().P() & Emu::P_Z_FLAG);
}

TEST_F(TestLogic, EOR)
{
	asse.Assemble(R"(
		eor #$80
		eor #$80
	)");

	bus.GetCpu().SetA(0);
	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().A(), 0x80);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_N_FLAG);

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().A(), 0);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_Z_FLAG);
	ASSERT_FALSE(bus.GetCpu().P() & Emu::P_N_FLAG);

}

TEST_F(TestLogic, ORA)
{
	asse.Assemble(R"(
		ora #$80
		ora #$00
	)");
	bus.GetCpu().SetA(0x01);
	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().A(), 0x81);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_N_FLAG);

	bus.GetCpu().SetA(0);
	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().A(), 0);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_Z_FLAG);
	ASSERT_FALSE(bus.GetCpu().P() & Emu::P_N_FLAG);
}

TEST_F(TestLogic, BIT)
{
	asse.Assemble(R"(
		bit 2
		&2 $c0
	)");

	clearCycles();

	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_Z_FLAG);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_V_FLAG);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_N_FLAG);

}

