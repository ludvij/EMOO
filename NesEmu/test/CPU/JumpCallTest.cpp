#include "pch.hpp"

#include "../TestConfig.hpp"

class TestJumpCall : public TestFixture {};


TEST_F(TestJumpCall, JMP)
{
	asse.Assemble(R"(
		ldx #23
		jmp $0005
		inx
		jmp $0040
	)");

	clearCycles(2 + 3);
	clearCycles(2);
	ASSERT_EQ(console.GetCpu().X(), 24);

	clearCycles(3);
	ASSERT_EQ(console.GetCpu().PC(), 0x40);
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
		jsr $0003
		rts
	)");
	

	clearCycles(6);
	clearCycles(6);
	ASSERT_EQ(3, console.GetCpu().PC());
	
}