
#include "../TestConfig.hpp"

class TestIncrementDecrement : public TestFixture
{

};

TEST_F(TestIncrementDecrement, INC)
{
	console.GetBus().Write(0x23, 0x7f);
	console.GetBus().Write(0x24, 0xfe);
	asse.Assemble(R"(
		ldx #$01
		inc $23
		inc $23,x
		inc $0024
		inc $0023,x
	)");

	clearCycles(2); // ldx
	clearCycles(5); // inc zpi
	ASSERT_EQ(console.GetBus().Read(0x23), 0x80);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);
	clearCycles(6); // inc zpx
	ASSERT_EQ(console.GetBus().Read(0x24), 0xff);
	clearCycles(6); // inc abs
	ASSERT_EQ(console.GetBus().Read(0x24), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
	clearCycles(7); // inc abx
	ASSERT_EQ(console.GetBus().Read(0x24), 1);
}

TEST_F(TestIncrementDecrement, INX)
{
	asse.Assemble(R"(
		ldx #$7f
		inx
		ldx #$fe
		inx
		inx
	)");

	clearCycles(2); // ldx
	clearCycles(2); // inx
	ASSERT_EQ(console.GetCpu().X(), 0x80);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);
	clearCycles(2); // ldx
	clearCycles(2); // inx
	ASSERT_EQ(console.GetCpu().X(), 0xff);
	clearCycles(2); // inx
	ASSERT_EQ(console.GetCpu().X(), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
}

TEST_F(TestIncrementDecrement, INY)
{
	asse.Assemble(R"(
		ldy #$7f
		iny
		ldy #$fe
		iny
		iny
	)");

	clearCycles(2); // ldy
	clearCycles(2); // iny
	ASSERT_EQ(console.GetCpu().Y(), 0x80);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);
	clearCycles(2); // ldy
	clearCycles(2); // iny
	ASSERT_EQ(console.GetCpu().Y(), 0xff);
	clearCycles(2); // iny
	ASSERT_EQ(console.GetCpu().Y(), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
}

TEST_F(TestIncrementDecrement, DEC)
{
	console.GetBus().Write(0x23, 0x81);
	console.GetBus().Write(0x24, 0x01);
	asse.Assemble(R"(
		ldx #$01
		dec $23
		dec $23,x
		dec $0024
		dec $0023,x
	)");

	clearCycles(2); // ldx
	clearCycles(5); // dec zpi
	ASSERT_EQ(console.GetBus().Read(0x23), 0x80);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);
	clearCycles(6); // dec zpx
	ASSERT_EQ(console.GetBus().Read(0x24), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
	clearCycles(6); // dec abs
	ASSERT_EQ(console.GetBus().Read(0x24), 0xff);
	clearCycles(7); // dec abx
	ASSERT_EQ(console.GetBus().Read(0x24), 0xfe);
}

TEST_F(TestIncrementDecrement, DEX)
{
	asse.Assemble(R"(
		ldx #$81
		dex
		ldx #$01
		dex
		dex
	)");

	clearCycles(2); // ldx
	clearCycles(2); // dex
	ASSERT_EQ(console.GetCpu().X(), 0x80);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);
	clearCycles(2); // ldx
	clearCycles(2); // dex
	ASSERT_EQ(console.GetCpu().X(), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
	clearCycles(2); // dex
	ASSERT_EQ(console.GetCpu().X(), 0xff);
}

TEST_F(TestIncrementDecrement, DEY)
{
	asse.Assemble(R"(
		ldy #$81
		dey
		ldy #$01
		dey
		dey
	)");

	clearCycles(2); // ldy
	clearCycles(2); // dey
	ASSERT_EQ(console.GetCpu().Y(), 0x80);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::N);
	clearCycles(2); // ldy
	clearCycles(2); // dey
	ASSERT_EQ(console.GetCpu().Y(), 0);
	ASSERT_TRUE(console.GetCpu().P() & Emu::ProcessorStatus::Flags::Z);
	clearCycles(2); // dey
	ASSERT_EQ(console.GetCpu().Y(), 0xff);
}