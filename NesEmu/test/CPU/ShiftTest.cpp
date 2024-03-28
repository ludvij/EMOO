#include "pch.hpp"

#include "../TestConfig.hpp"

class TestShift : public TestFixture
{

};

TEST_F(TestShift, ASL)
{
	asse.Assemble(R"(
		asl a
	)");

	console.GetCpu().SetA(0x80 + 0x40 + 13);

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().A(), 0x80 + 26);
	ASSERT_TRUE(console.GetCpu().P() & Emu::P_C_FLAG);
	ASSERT_TRUE(console.GetCpu().P() & Emu::P_N_FLAG);
}


TEST_F(TestShift, ASL_WRITE_MEMORY)
{
	asse.Assemble(R"(
		asl 2
		&2 $cd
	)");


	ASSERT_NO_THROW(clearCycles());

	ASSERT_EQ(console.GetBus().Read(2), 0x80 + 26);
	ASSERT_TRUE(console.GetCpu().P() & Emu::P_C_FLAG);
	ASSERT_TRUE(console.GetCpu().P() & Emu::P_N_FLAG);
}

TEST_F(TestShift, LSR)
{
	asse.Assemble(R"(
		lsr a
	)");

	console.GetCpu().SetA(1);

	ASSERT_NO_THROW(clearCycles());

	ASSERT_EQ(console.GetCpu().A(), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::P_C_FLAG);
	ASSERT_TRUE(console.GetCpu().P() & Emu::P_Z_FLAG);
}

TEST_F(TestShift, ROL)
{
	asse.Assemble(R"(
		rol a
	)");

	console.GetCpu().SetA(0x81 + 0x40);

	ASSERT_NO_THROW(clearCycles());

	ASSERT_EQ(console.GetCpu().A(), 0x82);
	ASSERT_TRUE(console.GetCpu().P() & Emu::P_C_FLAG);
	ASSERT_TRUE(console.GetCpu().P() & Emu::P_N_FLAG);

}

TEST_F(TestShift, ROR)
{
	asse.Assemble(R"(
		ror a
	)");

	console.GetCpu().SetA(0x81);
	console.GetCpu().SetP(Emu::P_C_FLAG);

	ASSERT_NO_THROW(clearCycles());

	ASSERT_EQ(console.GetCpu().A(), 0x40 + 0x80);
	ASSERT_TRUE(console.GetCpu().P() & Emu::P_C_FLAG);
	ASSERT_TRUE(console.GetCpu().P() & Emu::P_N_FLAG);
}