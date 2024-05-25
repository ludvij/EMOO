
#include "../TestConfig.hpp"


class TestLoadStore : public TestFixture
{

};

TEST_F(TestLoadStore, LDA)
{
	asse.Assemble(R"(
		lda #$17
		lda #$80
		lda #$00
	)");

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().A(), 23);

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().A(), 0x80);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().A(), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
}

TEST_F(TestLoadStore, LDX)
{
	asse.Assemble(R"(
		ldx #$17
		ldx #$80
		ldx #$00
	)");

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().X(), 23);

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().X(), 0x80);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().X(), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
}

TEST_F(TestLoadStore, LDY)
{
	asse.Assemble(R"(
		ldy #$17
		ldy #$80
		ldy #$00
	)");

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().Y(), 23);

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().Y(), 0x80);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);

	clearCycles(2);

	ASSERT_EQ(console.GetCpu().Y(), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
}

TEST_F(TestLoadStore, STA)
{
	asse.Assemble(R"(
		lda #$17
		sta $1212
	)");

	clearCycles(2 + 4);

	ASSERT_EQ(console.GetBus().Read(0x1212), 23);
}

TEST_F(TestLoadStore, STX)
{
	console.GetBus().Write(23, 23);
	asse.Assemble(R"(
		ldy #$0A
		ldx $0D,y
		stx $1212
	)");

	clearCycles(2 + 4 + 4);

	ASSERT_EQ(console.GetBus().Read(0x1212), 23);
}

TEST_F(TestLoadStore, STY)
{
	console.GetBus().Write(23, 23);
	asse.Assemble(R"(
		ldx #$0A
		ldy $0D,x
		sty $1212
	)");

	clearCycles(2 + 4 + 4);

	ASSERT_EQ(console.GetBus().Read(0x1212), 23);
}