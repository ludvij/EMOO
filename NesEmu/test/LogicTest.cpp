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

	mem[2] = 0x29;
	mem[3] = 12;
	cpu.SetA(12);

	clearCycles(2);

	ASSERT_EQ(cpu.A(), 12);
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

