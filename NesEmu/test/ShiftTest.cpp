#include "pch.hpp"

#include <gtest/gtest.h>

class TestShift : public testing::Test
{
protected:
	Emu::CPU cpu;
	Emu::Bus bus;
	Emu::u8* mem = nullptr;

	void SetUp() override
	{
		cpu = bus.GetCpu();
		mem = bus.GetMemory();
		cpu.SetP(0);
		// clear reset cycles
		clearCycles(8);
	}

	void clearCycles(int x = 1)
	{
		for (int i = 0; i < x; ++i)
		{
			cpu.Step();
		}
	}
};

TEST_F(TestShift, ASL)
{
	mem[0] = 0x0A;

	cpu.SetA(0x80 + 0x40 + 13);

	ASSERT_NO_THROW(clearCycles());

	ASSERT_EQ(cpu.A(), 0x80 + 26);
	ASSERT_TRUE(cpu.P() & Emu::P_C_FLAG);
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG);
}


TEST_F(TestShift, ASL_WRITE_MEMORY)
{
	mem[0] = 0x06;
	mem[1] = 2;
	mem[2] = 0x80 + 0x40 + 13;


	ASSERT_NO_THROW(clearCycles());

	ASSERT_EQ(mem[2], 0x80 + 26);
	ASSERT_TRUE(cpu.P() & Emu::P_C_FLAG);
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG);
}

TEST_F(TestShift, LSR)
{
	mem[0] = 0x4A;

	cpu.SetA(1);

	ASSERT_NO_THROW(clearCycles());

	ASSERT_EQ(cpu.A(), 0);
	ASSERT_TRUE(cpu.P() & Emu::P_C_FLAG);
	ASSERT_TRUE(cpu.P() & Emu::P_Z_FLAG);
}

TEST_F(TestShift, ROL)
{
	mem[0] = 0x2A;

	cpu.SetA(0x81 + 0x40);

	ASSERT_NO_THROW(clearCycles());

	ASSERT_EQ(cpu.A(), 0x82);
	ASSERT_TRUE(cpu.P() & Emu::P_C_FLAG);
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG);

}

TEST_F(TestShift, ROR)
{
	mem[0] = 0x6A;

	cpu.SetA(0x81);
	cpu.SetP(Emu::P_C_FLAG);

	ASSERT_NO_THROW(clearCycles());

	ASSERT_EQ(cpu.A(), 0x40 + 0x80);
	ASSERT_TRUE(cpu.P() & Emu::P_C_FLAG);
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG);
}