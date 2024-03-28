#include "pch.hpp"

#include "../TestConfig.hpp"

class TestScroll : public TestFixture {};


TEST_F(TestScroll, Full) 
{
	asse.Assemble(R"(
		LDA #$00
		STA $2000 
		LDA $2002
		LDA #$7D
		STA $2005
		LDA #$5E
		STA $2005
		LDA #$3D
		STA $2006
		LDA #$F0
		STA $2006
	)");
	// LDA #$00
	// STA $2000
	clearCycles(2 + 4);
	ASSERT_EQ(console.GetPpu().T(), 0);
	// LDA $2002
	clearCycles(2);
	ASSERT_EQ(console.GetPpu().T(), 0);
	ASSERT_EQ(console.GetPpu().W(), 0);
	// LDA #$7D
	// STA $2005
	clearCycles(2 + 4);
	ASSERT_EQ(console.GetPpu().T(), 0b000'00'00000'01111);
	ASSERT_EQ(console.GetPpu().X(), 0b101);
	ASSERT_EQ(console.GetPpu().W(), 1);
	// LDA #$5E
	// STA $2005
	clearCycles(2 + 4);
	ASSERT_EQ(console.GetPpu().T(), 0b110'00'01011'01111);
	ASSERT_EQ(console.GetPpu().X(), 0b101);
	ASSERT_EQ(console.GetPpu().W(), 0);	
	// LDA #$3D
	// STA $2006
	clearCycles(2 + 4);
	ASSERT_EQ(console.GetPpu().T(), 0b011'11'01011'01111);
	ASSERT_EQ(console.GetPpu().X(), 0b101);
	ASSERT_EQ(console.GetPpu().W(), 1);	
	// LDA #$F0
	// STA $2006
	clearCycles(2 + 4);
	ASSERT_EQ(console.GetPpu().T(), 0b011'11'01111'10000);
	ASSERT_EQ(console.GetPpu().V(), 0b011'11'01111'10000);
	ASSERT_EQ(console.GetPpu().X(), 0b101);
	ASSERT_EQ(console.GetPpu().W(), 0);	
}