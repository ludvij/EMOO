
#include "../TestConfig.hpp"

class TestMMIO : public TestFixture
{
};

TEST_F(TestMMIO, CONTROL)
{
	asse.Assemble(R"(
		LDA #$00
		STA $2000
		LDA #$23
		STA $2000
		LDA #$56
		STA $2000
	)");

	run_instruction();
	run_instruction();
	ASSERT_EQ(console.GetPpu().CpuPeek(0x2000), 0x00);
	ASSERT_EQ(console.GetPpu().T(), 0x00);

	run_instruction();
	run_instruction();
	const u16 t1 = ( 0x23 & 0x03 ) << 10;
	ASSERT_EQ(console.GetPpu().CpuPeek(0x2000), 0x23);
	ASSERT_EQ(console.GetPpu().T(), t1);

	run_instruction();
	run_instruction();
	const u16 t2 = ( 0x56 & 0x03 ) << 10;
	ASSERT_EQ(console.GetPpu().CpuPeek(0x2000), 0x56);
	ASSERT_EQ(console.GetPpu().T(), t2 | ( t1 & ~0x0C00 ));
}

TEST_F(TestMMIO, ADDRESS_DATA)
{
	// store address i.e $0821
	// store accumulator into data
	// restore address, since writing to data increments address
	// change ppu increment mode
	// increment address by writing to ppu
	// reset address and test buffer read
	asse.Assemble(R"(
		LDA #$61
		STA $2006
		LDA #$08
		STA $2006
		LDA #$23
		STA $2007

		LDA #$21
		STA $2006
		LDA #$08
		STA $2006

		LDA #$04
		STA $2000		
		STA $2007

		LDA #$21
		STA $2006
		LDA #$08
		STA $2006
		LDA $2007
		LDA $2007
	)");
	run_instruction(2);
	ASSERT_EQ(console.GetPpu().V(), 0);
	// check that address has been mirrored down by $3F
	ASSERT_EQ(console.GetPpu().T(), 0x21 << 8);

	run_instruction(3);
	ASSERT_EQ(console.GetPpu().V(), 0x2108);
	run_instruction();
	ASSERT_EQ(console.GetPpu().V(), 0x2108 + 1);
	ASSERT_EQ(console.GetPpu().CpuPeek(0x2007), 0);
	run_instruction(4);
	ASSERT_EQ(console.GetPpu().CpuPeek(0x2007), 0x23);
	run_instruction(3);
	ASSERT_EQ(console.GetPpu().V(), 0x2108 + 32);
	run_instruction(5);
	ASSERT_EQ(console.GetCpu().A(), 0);
	run_instruction();
	ASSERT_EQ(console.GetCpu().A(), 0x04);
}

TEST_F(TestMMIO, SCROLL)
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

	run_instruction();// LDA #$00
	run_instruction();// STA $2000
	ASSERT_EQ(console.GetPpu().T(), 0);

	run_instruction();// LDA $2002
	ASSERT_EQ(console.GetPpu().T(), 0);
	ASSERT_EQ(console.GetPpu().W(), 0);

	run_instruction();// LDA #$7D
	run_instruction();// STA $2005
	ASSERT_EQ(console.GetPpu().T(), 0b000'00'00000'01111);
	ASSERT_EQ(console.GetPpu().X(), 0b101);
	ASSERT_EQ(console.GetPpu().W(), 1);

	run_instruction();// LDA #$5E
	run_instruction();// STA $2005
	ASSERT_EQ(console.GetPpu().T(), 0b110'00'01011'01111);
	ASSERT_EQ(console.GetPpu().X(), 0b101);
	ASSERT_EQ(console.GetPpu().W(), 0);

	run_instruction();// LDA #$3D
	run_instruction();// STA $2006
	ASSERT_EQ(console.GetPpu().T(), 0b011'11'01011'01111);
	ASSERT_EQ(console.GetPpu().X(), 0b101);
	ASSERT_EQ(console.GetPpu().W(), 1);

	run_instruction();// LDA #$F0
	run_instruction();// STA $2006
	ASSERT_EQ(console.GetPpu().T(), 0b011'11'01111'10000);
	ASSERT_EQ(console.GetPpu().V(), 0b011'11'01111'10000);
	ASSERT_EQ(console.GetPpu().X(), 0b101);
	ASSERT_EQ(console.GetPpu().W(), 0);
}

//TEST_F(TestMMIO, DMA)
//{
//	asse.Assemble(R"(
//		STA $4014
//	)");
//
//}