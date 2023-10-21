#include "pch.hpp"

#include "TestConfig.hpp"


class TestStatusFlagChange : public TestFixture
{
	
};

TEST_F(TestStatusFlagChange, SEC)
{
	asse.Assemble("sec");

	clearCycles(2);

	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_C_FLAG );
}


TEST_F(TestStatusFlagChange, CLC)
{
	asse.Assemble("sec\nclc");

	clearCycles(4);

	ASSERT_FALSE(bus.GetCpu().P() & Emu::P_C_FLAG );
}

TEST_F(TestStatusFlagChange, SED)
{
	asse.Assemble("sed");

	clearCycles(2);

	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_D_FLAG );
}


TEST_F(TestStatusFlagChange, CLD)
{
	asse.Assemble("sed\ncld");

	clearCycles(4);

	ASSERT_FALSE(bus.GetCpu().P() & Emu::P_D_FLAG );
}

TEST_F(TestStatusFlagChange, SEI)
{
	asse.Assemble("sei");

	clearCycles(2);

	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_I_FLAG );
}


TEST_F(TestStatusFlagChange, CLI)
{
	asse.Assemble("sei\ncli");

	clearCycles(4);

	ASSERT_FALSE(bus.GetCpu().P() & Emu::P_I_FLAG );
}

TEST_F(TestStatusFlagChange, CLV)
{
	bus.GetCpu().SetP(Emu::P_V_FLAG);
	asse.Assemble("clv");

	clearCycles(2);

	ASSERT_FALSE(bus.GetCpu().P() & Emu::P_V_FLAG );
}