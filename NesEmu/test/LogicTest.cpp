#include "pch.hpp"
#include "internals/Bus.hpp"

#include <gtest/gtest.h>


class TestLogic : public testing::Test
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

TEST_F(TestLogic, AND)
{
	mem[0] = 0x29;
	mem[1] = 12;
	cpu.SetA(0);
	clearCycles();

	ASSERT_EQ(cpu.A(), 0);
	ASSERT_TRUE(cpu.P() & Emu::P_Z_FLAG);

	mem[2] = 0x29;
	mem[3] = 0x80 + 0x40;
	cpu.SetA(0x80);

	clearCycles(2);

	ASSERT_EQ(cpu.A(), 0x80);
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG);
	ASSERT_FALSE(cpu.P() & Emu::P_Z_FLAG);
}

TEST_F(TestLogic, EOR)
{
	mem[0] = 0x49;
	mem[1] = 0x80;
	cpu.SetA(0);
	clearCycles();

	ASSERT_EQ(cpu.A(), 0x80);
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG);

	mem[2] = 0x49;
	mem[3] = 0x80;

	clearCycles(2);

	ASSERT_EQ(cpu.A(), 0);
	ASSERT_TRUE(cpu.P() & Emu::P_Z_FLAG);
	ASSERT_FALSE(cpu.P() & Emu::P_N_FLAG);

}

TEST_F(TestLogic, ORA)
{
	mem[0] = 0x09;
	mem[1] = 0x80;
	cpu.SetA(0x01);
	clearCycles();

	ASSERT_EQ(cpu.A(), 0x81);
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG);

	mem[2] = 0x09;
	mem[3] = 0;

	cpu.SetA(0);
	clearCycles(2);

	ASSERT_EQ(cpu.A(), 0);
	ASSERT_TRUE(cpu.P() & Emu::P_Z_FLAG);
	ASSERT_FALSE(cpu.P() & Emu::P_N_FLAG);
}

TEST_F(TestLogic, BIT)
{
	mem[0] = 0x24;
	mem[1] = 2;
	mem[2] = 0x40 + 0x80;

	clearCycles();

	ASSERT_TRUE(cpu.P() & Emu::P_Z_FLAG);
	ASSERT_TRUE(cpu.P() & Emu::P_V_FLAG);
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG);

}

