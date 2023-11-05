#include "pch.hpp"

#include "TestConfig.hpp"

class TestShift : public TestFixture
{

};

TEST_F(TestShift, ASL)
{
	asse.Assemble(R"(
		asl a
	)");

	cpu.SetA(0x80 + 0x40 + 13);

	clearCycles(2);

	ASSERT_EQ(cpu.A(), 0x80 + 26);
	ASSERT_TRUE(cpu.P() & Emu::P_C_FLAG);
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG);
}


TEST_F(TestShift, ASL_WRITE_MEMORY)
{
	asse.Assemble(R"(
		asl 2
		&2 $cd
	)");


	ASSERT_NO_THROW(clearCycles());

	ASSERT_EQ(memory.Read(2), 0x80 + 26);
	ASSERT_TRUE(cpu.P() & Emu::P_C_FLAG);
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG);
}

TEST_F(TestShift, LSR)
{
	asse.Assemble(R"(
		lsr a
	)");

	cpu.SetA(1);

	ASSERT_NO_THROW(clearCycles());

	ASSERT_EQ(cpu.A(), 0);
	ASSERT_TRUE(cpu.P() & Emu::P_C_FLAG);
	ASSERT_TRUE(cpu.P() & Emu::P_Z_FLAG);
}

TEST_F(TestShift, ROL)
{
	asse.Assemble(R"(
		rol a
	)");

	cpu.SetA(0x81 + 0x40);

	ASSERT_NO_THROW(clearCycles());

	ASSERT_EQ(cpu.A(), 0x82);
	ASSERT_TRUE(cpu.P() & Emu::P_C_FLAG);
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG);

}

TEST_F(TestShift, ROR)
{
	asse.Assemble(R"(
		ror a
	)");

	cpu.SetA(0x81);
	cpu.SetP(Emu::P_C_FLAG);

	ASSERT_NO_THROW(clearCycles());

	ASSERT_EQ(cpu.A(), 0x40 + 0x80);
	ASSERT_TRUE(cpu.P() & Emu::P_C_FLAG);
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG);
}