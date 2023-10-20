#include "pch.hpp"

#include "TestConfig.hpp"


class TestLoadStore : public TestFixture
{
	
};

TEST_F(TestLoadStore, LDA)
{
	asse.Assemble(R"(
		lda #23
		lda #$80
		lda #0
	)");

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().A(), 23);

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().A(), 0x80);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_N_FLAG);

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().A(), 0);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_Z_FLAG);
}

TEST_F(TestLoadStore, LDX)
{
	asse.Assemble(R"(
		ldx #23
		ldx #$80
		ldx #0
	)");

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().X(), 23);

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().X(), 0x80);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_N_FLAG);

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().X(), 0);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_Z_FLAG);
}

TEST_F(TestLoadStore, LDY)
{
	asse.Assemble(R"(
		ldy #23
		ldy #$80
		ldy #0
	)");

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().Y(), 23);

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().Y(), 0x80);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_N_FLAG);

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().Y(), 0);
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_Z_FLAG);
}

TEST_F(TestLoadStore, STA)
{
	asse.Assemble(R"(
		lda #23
		sta $1212
	)");

	clearCycles(2 + 4);

	ASSERT_EQ(bus.Read(0x1212), 23);
}

TEST_F(TestLoadStore, STX)
{
	asse.Assemble(R"(
		ldy #10
		ldx 13,y
		stx $1212
		&23 23
	)");

	clearCycles(2 + 4 + 4);

	ASSERT_EQ(bus.Read(0x1212), 23);
}

TEST_F(TestLoadStore, STY)
{
	asse.Assemble(R"(
		ldx #10
		ldy 13,x
		sty $1212
		&23 23
	)");

	clearCycles(2 + 4 + 4);

	ASSERT_EQ(bus.Read(0x1212), 23);
}