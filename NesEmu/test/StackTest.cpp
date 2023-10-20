#include "pch.hpp"

#include <gtest/gtest.h>


class TestStack : public testing::Test
{
protected:
	Emu::CPU cpu;
	Emu::Bus bus;
	Emu::u8* mem = nullptr;

	void SetUp() override
	{
		cpu = bus.GetCpu();
		mem = bus.GetMemory();
		cpu.SetPC(0);
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

TEST_F(TestStack, TSX)
{
	A6502::ToyAssembler::Get().Assemble(R"(
		tsx
		tsx
	)", mem);

	cpu.SetS(0x80);

	clearCycles(2);

	ASSERT_EQ(cpu.X(), 0x80);
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG);

	cpu.SetS(0);

	clearCycles(2);

	ASSERT_EQ(cpu.X(), 0x00);
	ASSERT_TRUE(cpu.P() & Emu::P_Z_FLAG);
}

TEST_F(TestStack, TXS)
{
	A6502::ToyAssembler::Get().Assemble(R"(
		txs
		txs
	)", mem);

	cpu.SetX(0x80);

	clearCycles(2);

	ASSERT_EQ(cpu.S(), 0x80);
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG);

	cpu.SetX(0);

	clearCycles(2);

	ASSERT_EQ(cpu.S(), 0x00);
	ASSERT_TRUE(cpu.P() & Emu::P_Z_FLAG);
}

TEST_F(TestStack, STACK_PUSH)
{
	A6502::ToyAssembler::Get().Assemble(R"(
		pha
		php
		pha
		php
	)", mem);
	Emu::u8 base = cpu.S();

	cpu.SetA(12);
	cpu.SetP(23);

	clearCycles(3);
	ASSERT_EQ(mem[0x0100 + cpu.S() + 1], 12);
	ASSERT_EQ(cpu.S() + 1, base);
	clearCycles(3);
	ASSERT_EQ(mem[0x0100 + cpu.S() + 1], 23);
	ASSERT_EQ(cpu.S() + 2, base);
	clearCycles(3);
	ASSERT_EQ(mem[0x0100 + cpu.S() + 1], 12);
	ASSERT_EQ(cpu.S() + 3, base);
	clearCycles(3);
	ASSERT_EQ(mem[0x0100 + cpu.S() + 1], 23);
	ASSERT_EQ(cpu.S() + 4, base);
}

TEST_F(TestStack, STACK_POP)
{
	A6502::ToyAssembler::Get().Assemble(R"(
        php
        php
		pha
		pha
		pla
		pla
		plp
		plp
	)", mem);

	cpu.SetP(0x23);
	clearCycles(3);
	// 0x23, -
	cpu.SetP(12);
	clearCycles(3);
	// 12, 0x23, -
	cpu.SetA(0x80);
	clearCycles(3);
	// 0x80, 12, 0x23, -
	cpu.SetA(0);
	clearCycles(3);
	// 0, 0x80, 12, 0x23, -

	Emu::u8 base = cpu.S();
	clearCycles(4);
	// 0x80, 12, 0x23, -
	ASSERT_EQ(0, cpu.A());
	ASSERT_EQ(static_cast<Emu::u8>(cpu.S() - 1), base);
	ASSERT_TRUE(cpu.P() & Emu::P_Z_FLAG);

	clearCycles(4);
	// 12, 0x23, -
	ASSERT_EQ(0x80, cpu.A());
	ASSERT_EQ(static_cast<Emu::u8>(cpu.S() - 2), base);
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG);


	clearCycles(4);
	// 0x23, -
	ASSERT_EQ(12, cpu.P());
	ASSERT_EQ(static_cast<Emu::u8>(cpu.S() - 3), base);


	clearCycles(4);
	// -
	ASSERT_EQ(0x23, cpu.P());
	ASSERT_EQ(static_cast<Emu::u8>(cpu.S() - 4), base);

}