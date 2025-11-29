
#include "../TestConfig.hpp"

class TestJumpCall : public TestFixture
{
};


TEST_F(TestJumpCall, JMP)
{
	console.GetBus().Write(0x0123, 0x17);
	console.GetBus().Write(0x00FF, 0x23);
	asse.Assemble(R"(
		ldx #$17
		jmp $0005
		inx
		jmp $0040
		.at $40
		jmp ($0123)
		.at $17
		jmp ($00FF)
	)");

	clearCycles(2 + 3);
	clearCycles(2);
	ASSERT_EQ(console.GetCpu().X(), 24);

	clearCycles(3);
	ASSERT_EQ(console.GetCpu().PC(), 0x40);

	run_instruction();
	ASSERT_EQ(console.GetCpu().PC(), 0x17);

	run_instruction();
	const u16 result = ( console.GetBus().Read(0) << 8 | 0x23 );
	ASSERT_EQ(console.GetCpu().PC(), result);

}

TEST_F(TestJumpCall, JSR)
{
	asse.Assemble(R"(
		jsr $0013
	)");

	clearCycles(6);
	ASSERT_EQ(console.GetBus().Read(0x1fc), 2);
	ASSERT_EQ(console.GetCpu().PC(), 0x13);
}

TEST_F(TestJumpCall, RTS)
{
	asse.Assemble(R"(
		jsr $0005
		lda #$23
		rts
	)");


	run_instruction(); // JSR
	run_instruction(); // RTS
	ASSERT_EQ(3, console.GetCpu().PC());
	run_instruction(); // LDA
	ASSERT_EQ(console.GetCpu().A(), 0x23);

}