#include "pch.hpp"

#include <gtest/gtest.h>

#include "utils/Assembler.hpp"

class TestArithmetic : public testing::Test
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


TEST_F(TestArithmetic, ADC_IMM_N)
{
	auto a = A6502::Assembler(bus, true);
	a.Assemble(R"(
	:test
		adc #$1212
		lda (23,x)
		jmp (2200)
		lda 23
		bcc test
		bcc res
		adc #%011100
	:res
	)");
	mem[0] = 0x69;
	mem[1] = 0x80;
	cpu.SetA(10);

	clearCycles();

	ASSERT_EQ(cpu.A(), 0x80 + 10);
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG);
}

TEST_F(TestArithmetic, ADC_ZPI_C)
{
	mem[0] = 0x65;
	mem[1] = 2;
	mem[2] = 0x80;
	cpu.SetA(0x80);

	clearCycles();

	ASSERT_EQ(cpu.A(), 0);
	ASSERT_TRUE(cpu.P() & Emu::P_C_FLAG);
}

TEST_F(TestArithmetic, ADC_ZPX_Z)
{
	mem[0] = 0x75;
	mem[1] = 1;
	mem[2] = 0x80;
	cpu.SetA(0x80);
	cpu.SetX(1);

	clearCycles();

	ASSERT_EQ(cpu.A(), 0);
	ASSERT_TRUE(cpu.P() & Emu::P_Z_FLAG);
}

TEST_F(TestArithmetic, ADC_ABS_C)
{
	mem[0] = 0x6D;
	mem[1] = 3;
	mem[2] = 0;
	mem[3] = 13;

	cpu.SetP(Emu::P_C_FLAG);
	cpu.SetA(12);

	clearCycles();

	ASSERT_EQ(cpu.A(), 26);
	ASSERT_FALSE(cpu.P() & Emu::P_C_FLAG);
}

TEST_F(TestArithmetic, ADC_ABX_OOPS)
{
	mem[0] = 0x7D;
	mem[1] = 0xff;
	mem[2] = 0x00;
	mem[0x0100] = 13;

	cpu.SetP(Emu::P_C_FLAG);
	cpu.SetA(12);
	cpu.SetX(1);

	clearCycles();

	ASSERT_EQ(cpu.A(), 26);
	// check for oopscycles
	ASSERT_EQ(cpu.GetCycles(), 4);
}

TEST_F(TestArithmetic, ADC_ABY_OOPS)
{
	mem[0] = 0x79;
	mem[1] = 0xff;
	mem[2] = 0x00;
	mem[0x0100] = 13;

	cpu.SetP(Emu::P_C_FLAG);
	cpu.SetA(12);
	cpu.SetY(1);

	clearCycles();

	ASSERT_EQ(cpu.A(), 26);
	// check for oopscycles
	ASSERT_EQ(cpu.GetCycles(), 4);
}

// TODO: test ADC indirect