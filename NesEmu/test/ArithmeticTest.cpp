#include "pch.hpp"

#include "TestConfig.hpp"


class TestArithmetic : public TestFixture
{

};


TEST_F(TestArithmetic, ADC_IMM_N)
{
	asse.Assemble(R"(
		adc #$80
	)");
	bus.GetCpu().SetA(10);

	clearCycles();

	ASSERT_EQ(bus.GetCpu().A(), 0x80 + 10);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_N_FLAG);
}

TEST_F(TestArithmetic, ADC_ZPI_C)
{
	asse.Assemble(R"(
		adc 2
		$80
	)");
	bus.GetCpu().SetA(0x80);

	clearCycles();

	ASSERT_EQ(bus.GetCpu().A(), 0);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_C_FLAG);
}

TEST_F(TestArithmetic, ADC_ZPX_Z)
{
	asse.Assemble(R"(
		adc 2
		$80
	)");
	bus.GetCpu().SetA(0x80);
	bus.GetCpu().SetX(1);

	clearCycles();

	ASSERT_EQ(bus.GetCpu().A(), 0);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_Z_FLAG);
}

TEST_F(TestArithmetic, ADC_ABS_C)
{
	asse.Assemble(R"(
		adc $0003
		13
	)");

	bus.GetCpu().SetP(Emu::P_C_FLAG);
	bus.GetCpu().SetA(12);

	clearCycles();

	ASSERT_EQ(bus.GetCpu().A(), 26);
	ASSERT_FALSE(bus.GetCpu().P() & Emu::P_C_FLAG);
}

TEST_F(TestArithmetic, ADC_ABX_OOPS)
{
	asse.Assemble(R"(
		adc $00ff, x
		&$0100 13
	)");
	bus.Write(0, 0x7D);
	bus.Write(1, 0xff);
	bus.Write(2, 0x00);
	bus.Write(0x0100, 13);

	bus.GetCpu().SetP(Emu::P_C_FLAG);
	bus.GetCpu().SetA(12);
	bus.GetCpu().SetX(1);

	clearCycles();

	ASSERT_EQ(bus.GetCpu().A(), 26);
	// check for oopscycles
	ASSERT_EQ(bus.GetCpu().GetCycles(), 4);
}

TEST_F(TestArithmetic, ADC_ABY_OOPS)
{
	asse.Assemble(R"(
		adc $00ff, y
		&$0100 13
	)");

	bus.GetCpu().SetP(Emu::P_C_FLAG);
	bus.GetCpu().SetA(12);
	bus.GetCpu().SetY(1);

	clearCycles();

	ASSERT_EQ(bus.GetCpu().A(), 26);
	// check for oopscycles
	ASSERT_EQ(bus.GetCpu().GetCycles(), 4);
}

TEST_F(TestArithmetic, ADC_INX)
{
	asse.Assemble(R"(
		adc (10, x)
		&2 23
		&12 2
		&13 0
	)");

	bus.GetCpu().SetA(12);
	bus.GetCpu().SetX(2);

	clearCycles(6);

	ASSERT_EQ(bus.GetCpu().A(), 12 + 23);
}

TEST_F(TestArithmetic, ADC_INY_NO_OOPS)
{
	asse.Assemble(R"(
		adc (10), y
		&4 23
		&10 2
		&11 0
	)");

	bus.GetCpu().SetA(12);
	bus.GetCpu().SetY(2);

	clearCycles(5);

	ASSERT_EQ(bus.GetCpu().A(), 12 + 23);
	ASSERT_EQ(bus.GetCpu().GetCycles(), 0);
}

TEST_F(TestArithmetic, ADC_INY_OOPS)
{
	asse.Assemble(R"(
		adc (10), y
		&10 $ff
		&11 0
		&$0101 23
	)");

	bus.GetCpu().SetA(12);
	bus.GetCpu().SetY(2);

	clearCycles(5);

	ASSERT_EQ(bus.GetCpu().A(), 12 + 23);
	ASSERT_EQ(bus.GetCpu().GetCycles(), 1);
}

TEST_F(TestArithmetic, ADC_V)
{
	asse.Assemble(R"(
		sec
		lda #$3f
		adc #$40

		clc
		lda #$01
		adc #$ff
	)");

	// should overflow
	clearCycles(2 + 2 + 2);

	ASSERT_EQ(bus.GetCpu().A(), 128);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_V_FLAG);

	clearCycles(2 + 2 + 2);

	ASSERT_EQ(bus.GetCpu().A(), 0);
	ASSERT_FALSE(bus.GetCpu().P() & Emu::P_V_FLAG);
}


TEST_F(TestArithmetic, SBC)
{
	asse.Assemble(R"(
		sec
		lda #23
		sbc #10

		clc
		lda #$82
		sbc #1

		sec
		lda #3
		sbc #3

		sec
		lda #3
		sbc #4
	)");

	clearCycles(2 + 2 + 2);
	ASSERT_EQ(bus.GetCpu().A(), 13);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_C_FLAG);

	clearCycles(2 + 2 + 2);
	ASSERT_EQ(bus.GetCpu().A(), 0x80);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_N_FLAG);

	clearCycles(2 + 2 + 2);
	ASSERT_EQ(bus.GetCpu().A(), 0);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_Z_FLAG);

	clearCycles(2 + 2 + 2);
	ASSERT_EQ(bus.GetCpu().A(), 0xff); // -1 in signed 8 bits is 0xff
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_V_FLAG);
}

TEST_F(TestArithmetic, CMP)
{
	asse.Assemble(R"(
		lda #22
		cmp #21

		lda #20
		cmp #20

		lda #$84
		cmp #$04
	)");
	
	clearCycles(2 + 2);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_C_FLAG);

	clearCycles(2 + 2);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_Z_FLAG);

	clearCycles(2 + 2);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_N_FLAG);
}
