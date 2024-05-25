
#include <gtest/gtest.h>


#include "../TestConfig.hpp"


class TestLogic : public TestFixture
{

};


TEST_F(TestLogic, AND)
{
	asse.Assemble(R"(
		and #$0C
		and #$c0
	)");
	console.GetCpu().SetA(0);
	clearCycles(2);

	ASSERT_EQ(console.GetCpu().A(), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);


	console.GetCpu().SetA(0x80);

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().A(), 0x80);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);
	ASSERT_FALSE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
}

TEST_F(TestLogic, EOR)
{
	asse.Assemble(R"(
		eor #$80
		eor #$80
	)");

	console.GetCpu().SetA(0);
	clearCycles(2);

	ASSERT_EQ(console.GetCpu().A(), 0x80);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().A(), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
	ASSERT_FALSE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);

}

TEST_F(TestLogic, ORA)
{
	asse.Assemble(R"(
		ora #$80
		ora #$00
	)");
	console.GetCpu().SetA(0x01);
	clearCycles(2);

	ASSERT_EQ(console.GetCpu().A(), 0x81);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);

	console.GetCpu().SetA(0);
	clearCycles(2);

	ASSERT_EQ(console.GetCpu().A(), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
	ASSERT_FALSE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);
}

TEST_F(TestLogic, BIT)
{
	console.GetBus().Write(2, 0xC0);
	asse.Assemble(R"(
		bit $02
	)");

	clearCycles();

	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::V);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);

}

