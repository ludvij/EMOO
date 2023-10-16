#include "pch.hpp"
#include "internals/Bus.hpp"

#include <gtest/gtest.h>


class TestFixture : public testing::Test
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

TEST_F(TestFixture, GENERAL_BRANCH_PAGECROSS)
{
	mem[0] = 0x90;
	mem[1] = -13;

	clearCycles();

	// pc is 2, and the offset is
	// -13 so offset + pc = -11
	// pc is u16 so it wraps to $0000 - 11 = $fff5
	ASSERT_EQ(cpu.PC(), 0xfff5);
	// page crossing
	ASSERT_EQ(cpu.GetCycles(), 3);
}

TEST_F(TestFixture, BCC_BRANCH)
{
	mem[0] = 0x90;
	mem[1] = 2;

	clearCycles();

	ASSERT_EQ(cpu.PC(), 4);
	// no page crossing
	ASSERT_EQ(cpu.GetCycles(), 2);
}

TEST_F(TestFixture, BCC_NOBRANCH)
{
	cpu.SetP(Emu::P_C_FLAG);
	mem[0] = 0x90;
	mem[1] = 2;

	clearCycles();

	// no branching
	ASSERT_EQ(cpu.PC(), 2);
	ASSERT_EQ(cpu.GetCycles(), 1);
}

TEST_F(TestFixture, BCS_BRANCH)
{
	cpu.SetP(Emu::P_C_FLAG);
	mem[0] = 0xB0;
	mem[1] = 2;

	clearCycles();

	ASSERT_EQ(cpu.PC(), 4);
	// no page crossing
	ASSERT_EQ(cpu.GetCycles(), 2);
}

TEST_F(TestFixture, BCS_NOBRANCH)
{
	mem[0] = 0xB0;
	mem[1] = 2;

	clearCycles();

	// no branching
	ASSERT_EQ(cpu.PC(), 2);
	ASSERT_EQ(cpu.GetCycles(), 1);
}

TEST_F(TestFixture, BEQ_BRANCH)
{
	cpu.SetP(Emu::P_Z_FLAG);
	mem[0] = 0xF0;
	mem[1] = 2;

	clearCycles();

	ASSERT_EQ(cpu.PC(), 4);
	// no page crossing
	ASSERT_EQ(cpu.GetCycles(), 2);
}

TEST_F(TestFixture, BEQ_NOBRANCH)
{
	mem[0] = 0xF0;
	mem[1] = 2;

	clearCycles();

	// no branching
	ASSERT_EQ(cpu.PC(), 2);
	ASSERT_EQ(cpu.GetCycles(), 1);
}
