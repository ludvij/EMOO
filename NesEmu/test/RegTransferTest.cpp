#include "pch.hpp"
#include "internals/Bus.hpp"

#include <gtest/gtest.h>


class TestRegTransfer : public testing::Test
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

// the same test 6 times, i refuse to code the same function
// 6 times, so have a macro
#define TRANSFER_TEST(fro, to, memPos)        \
	TEST_F(TestRegTransfer, T##fro##to)       \
	{                                         \
		mem[0] = memPos;                      \
		cpu.Set##fro(0x80);                   \
                                              \
		clearCycles(2);                       \
                                              \
		ASSERT_EQ(cpu.##to(), 0x80);          \
		ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG); \
                                              \
		mem[1] = memPos;                      \
		cpu.Set##fro(0);                      \
                                              \
		clearCycles(2);                       \
		ASSERT_EQ(cpu.##to(), 0);             \
		ASSERT_TRUE(cpu.P() & Emu::P_Z_FLAG); \
	}                                         \

TRANSFER_TEST(A, X, 0xAA);
TRANSFER_TEST(A, Y, 0xA8);
TRANSFER_TEST(S, X, 0xBA);
TRANSFER_TEST(X, A, 0x8A);
TRANSFER_TEST(Y, A, 0x98);
TRANSFER_TEST(X, S, 0x9A);