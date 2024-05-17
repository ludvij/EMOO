
#include "../TestConfig.hpp"


class TestStatusFlagChange : public TestFixture
{

};

TEST_F(TestStatusFlagChange, SEC)
{
	asse.Assemble("sec");

	clearCycles(2);

	ASSERT_TRUE(console.GetCpu().P() & Emu::Flag::C);
}


TEST_F(TestStatusFlagChange, CLC)
{
	asse.Assemble("sec\nclc");

	clearCycles(4);

	ASSERT_FALSE(console.GetCpu().P() & Emu::Flag::C);
}

TEST_F(TestStatusFlagChange, SED)
{
	asse.Assemble("sed");

	clearCycles(2);

	ASSERT_TRUE(console.GetCpu().P() & Emu::Flag::D);
}


TEST_F(TestStatusFlagChange, CLD)
{
	asse.Assemble("sed\ncld");

	clearCycles(4);

	ASSERT_FALSE(console.GetCpu().P() & Emu::Flag::D);
}

TEST_F(TestStatusFlagChange, SEI)
{
	asse.Assemble("sei");

	clearCycles(2);

	ASSERT_TRUE(console.GetCpu().P() & Emu::Flag::I);
}


TEST_F(TestStatusFlagChange, CLI)
{
	asse.Assemble("sei\ncli");

	clearCycles(4);

	ASSERT_FALSE(console.GetCpu().P() & Emu::Flag::I);
}

TEST_F(TestStatusFlagChange, CLV)
{
	console.GetCpu().SetP(Emu::Flag::V);
	asse.Assemble("clv");

	clearCycles(2);

	ASSERT_FALSE(console.GetCpu().P() & Emu::Flag::V);
}