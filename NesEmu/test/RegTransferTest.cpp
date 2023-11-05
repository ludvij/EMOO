#include "pch.hpp"

#include "TestConfig.hpp"


class TestRegTransfer : public TestFixture
{

};


TEST_F(TestRegTransfer, TAX)
{
	asse.Assemble(R"(
		tax
		tax
	)");

	cpu.SetA(0x80);

	clearCycles(2);

	ASSERT_EQ(cpu.X(), 0x80);          
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG); 
                                          
	cpu.SetA(0);                      
                                          
	clearCycles(2);                       
	ASSERT_EQ(cpu.X(), 0);             
	ASSERT_TRUE(cpu.P() & Emu::P_Z_FLAG); 
}

TEST_F(TestRegTransfer, TAY)
{
	asse.Assemble(R"(
		tay
		tay
	)");

	cpu.SetA(0x80);

	clearCycles(2);

	ASSERT_EQ(cpu.Y(), 0x80);          
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG); 
                                          
	cpu.SetA(0);                      
                                          
	clearCycles(2);                       
	ASSERT_EQ(cpu.Y(), 0);             
	ASSERT_TRUE(cpu.P() & Emu::P_Z_FLAG); 
}

TEST_F(TestRegTransfer, TXA)
{
	asse.Assemble(R"(
		txa
		txa
	)");

	cpu.SetX(0x80);

	clearCycles(2);

	ASSERT_EQ(cpu.A(), 0x80);          
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG); 
                                          
	cpu.SetX(0);                      
                                          
	clearCycles(2);                       
	ASSERT_EQ(cpu.A(), 0);             
	ASSERT_TRUE(cpu.P() & Emu::P_Z_FLAG); 
}

TEST_F(TestRegTransfer, TYA)
{
	asse.Assemble(R"(
		tya
		tya
	)");

	cpu.SetY(0x80);

	clearCycles(2);

	ASSERT_EQ(cpu.A(), 0x80);          
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG); 
                                          
	cpu.SetY(0);                      
                                          
	clearCycles(2);                       
	ASSERT_EQ(cpu.A(), 0);             
	ASSERT_TRUE(cpu.P() & Emu::P_Z_FLAG); 
}