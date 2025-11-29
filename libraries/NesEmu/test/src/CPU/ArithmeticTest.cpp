
#include "../TestConfig.hpp"


class TestArithmetic : public TestFixture
{

};


TEST_F(TestArithmetic, ADC_IMM_N)
{
	asse.Assemble(R"(
		adc #$80
	)");
	console.GetCpu().SetA(10);

	clearCycles();

	ASSERT_EQ(console.GetCpu().A(), 0x80 + 10);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);
}

TEST_F(TestArithmetic, ADC_ZPI_C)
{
	console.GetBus().Write(2, 0x80);
	asse.Assemble(R"(
		adc $02
	)");
	console.GetCpu().SetA(0x80);

	clearCycles();

	ASSERT_EQ(console.GetCpu().A(), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::C);
}

TEST_F(TestArithmetic, ADC_ZPX_Z)
{
	console.GetBus().Write(3, 0x80);

	asse.Assemble(R"(
		adc $02,x
	)");
	console.GetCpu().SetA(0x80);
	console.GetCpu().SetX(1);

	clearCycles();

	ASSERT_EQ(console.GetCpu().A(), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
}

TEST_F(TestArithmetic, ADC_ABS_C)
{
	console.GetBus().Write(3, 13);

	asse.Assemble(R"(
		adc $0003
	)");

	console.GetCpu().SetP(Emu::ProcessorStatus::Flags::C);
	console.GetCpu().SetA(12);

	clearCycles();

	ASSERT_EQ(console.GetCpu().A(), 26);
	ASSERT_FALSE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::C);
}

TEST_F(TestArithmetic, ADC_ABX_OOPS)
{
	console.GetBus().Write(0x0100, 13);

	asse.Assemble(R"(
		adc $00ff,x
	)");
	console.GetBus().Write(0x0100, 13);

	console.GetCpu().SetP(Emu::ProcessorStatus::Flags::C);
	console.GetCpu().SetA(12);
	console.GetCpu().SetX(1);

	clearCycles();

	ASSERT_EQ(console.GetCpu().A(), 26);
	// check for oopscycles
	ASSERT_EQ(console.GetCpu().GetCycles(), 4);
}

TEST_F(TestArithmetic, ADC_ABY_OOPS)
{
	console.GetBus().Write(0x0100, 13);
	asse.Assemble(R"(
		adc $00ff,y
	)");

	console.GetCpu().SetP(Emu::ProcessorStatus::Flags::C);
	console.GetCpu().SetA(12);
	console.GetCpu().SetY(1);

	clearCycles();

	ASSERT_EQ(console.GetCpu().A(), 26);
	// check for oopscycles
	ASSERT_EQ(console.GetCpu().GetCycles(), 4);
}

TEST_F(TestArithmetic, ADC_INX)
{
	console.GetBus().Write(0x000F, 23);
	console.GetBus().Write(0x000C, 0x0F);
	console.GetBus().Write(0x000D, 0x00);
	asse.Assemble(R"(
		adc ($000A,x)
	)");

	console.GetCpu().SetA(12);
	console.GetCpu().SetX(2);

	clearCycles(6);

	ASSERT_EQ(console.GetCpu().A(), 12 + 23);
}

TEST_F(TestArithmetic, ADC_INY_NO_OOPS)
{
	console.GetBus().Write(15, 23);
	console.GetBus().Write(10, 0x0D);
	console.GetBus().Write(11, 0x00);
	asse.Assemble(R"(
		adc ($000A),y
	)");

	console.GetCpu().SetA(12);
	console.GetCpu().SetY(2);

	clearCycles(5);

	ASSERT_EQ(console.GetCpu().A(), 12 + 23);
	ASSERT_EQ(console.GetCpu().GetCycles(), 0);
}

TEST_F(TestArithmetic, ADC_INY_OOPS)
{
	console.GetBus().Write(10, 0xFF);
	console.GetBus().Write(11, 0);
	console.GetBus().Write(0x0101, 23);
	asse.Assemble(R"(
		adc ($000A),y
	)");

	console.GetCpu().SetA(12);
	console.GetCpu().SetY(2);

	clearCycles(5);

	ASSERT_EQ(console.GetCpu().A(), 12 + 23);
	ASSERT_EQ(console.GetCpu().GetCycles(), 1);
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

	ASSERT_EQ(console.GetCpu().A(), 128);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::V);

	clearCycles(2 + 2 + 2);

	ASSERT_EQ(console.GetCpu().A(), 0);
	ASSERT_FALSE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::V);
}


TEST_F(TestArithmetic, SBC)
{
	asse.Assemble(R"(
		sec
		lda #$17
		sbc #$0A

		clc
		lda #$82
		sbc #$01

		sec
		lda #$03
		sbc #$03

		sec
		lda #$83
		sbc #$04
	)");

	clearCycles(2 + 2 + 2);
	ASSERT_EQ(console.GetCpu().A(), 13);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::C);

	clearCycles(2 + 2 + 2);
	ASSERT_EQ(console.GetCpu().A(), 0x80);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);

	clearCycles(2 + 2 + 2);
	ASSERT_EQ(console.GetCpu().A(), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);

	clearCycles(2 + 2 + 2);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::V);
	ASSERT_EQ(console.GetCpu().A(), 0x7f); // -1 in signed 8 bits is 0xff
}

TEST_F(TestArithmetic, CMP)
{
	asse.Assemble(R"(
		lda #$16
		cmp #$15

		lda #$14
		cmp #$14

		lda #$84
		cmp #$04
	)");

	clearCycles(2 + 2);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::C);

	clearCycles(2 + 2);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);

	clearCycles(2 + 2);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);
}

TEST_F(TestArithmetic, CPX)
{
	asse.Assemble(R"(
		ldx #$16
		cpx #$15

		ldx #$14
		cpx #$14

		ldx #$84
		cpx #$04
	)");

	clearCycles(2 + 2);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::C);

	clearCycles(2 + 2);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);

	clearCycles(2 + 2);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);
}

TEST_F(TestArithmetic, CPY)
{
	asse.Assemble(R"(
		ldy #$16
		cpy #$15

		ldy #$14
		cpy #$14

		ldy #$84
		cpy #$04
	)");

	clearCycles(2 + 2);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::C);

	clearCycles(2 + 2);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);

	clearCycles(2 + 2);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);
}

TEST_F(TestArithmetic, BIT)
{
	console.GetBus().Write(13, 0x40);
	console.GetBus().Write(14, 0x80);
	asse.Assemble(R"(
		lda #$80
		bit $0D
		
		lda #$80
		bit $0E
		
	)");

	clearCycles(2 + 3);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::V);

	clearCycles(2 + 3);
	ASSERT_FALSE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);
}
