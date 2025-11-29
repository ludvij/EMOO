
#include "../TestConfig.hpp"


class TestRegTransfer : public TestFixture
{

};


TEST_F(TestRegTransfer, TAX)
{
	asse.Assemble(R"(
		tax
		tax
	)");

	console.GetCpu().SetA(0x80);

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().X(), 0x80);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);

	console.GetCpu().SetA(0);

	clearCycles(2);
	ASSERT_EQ(console.GetCpu().X(), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
}

TEST_F(TestRegTransfer, TAY)
{
	asse.Assemble(R"(
		tay
		tay
	)");

	console.GetCpu().SetA(0x80);

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().Y(), 0x80);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);

	console.GetCpu().SetA(0);

	clearCycles(2);
	ASSERT_EQ(console.GetCpu().Y(), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
}

TEST_F(TestRegTransfer, TXA)
{
	asse.Assemble(R"(
		txa
		txa
	)");

	console.GetCpu().SetX(0x80);

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().A(), 0x80);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);

	console.GetCpu().SetX(0);

	clearCycles(2);
	ASSERT_EQ(console.GetCpu().A(), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
}

TEST_F(TestRegTransfer, TYA)
{
	asse.Assemble(R"(
		tya
		tya
	)");

	console.GetCpu().SetY(0x80);

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().A(), 0x80);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);

	console.GetCpu().SetY(0);

	clearCycles(2);
	ASSERT_EQ(console.GetCpu().A(), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
}