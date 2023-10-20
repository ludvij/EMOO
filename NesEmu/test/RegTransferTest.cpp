#include "pch.hpp"

#include <gtest/gtest.h>


class TestRegTransfer : public testing::Test
{
protected:
	Emu::CPU cpu;
	Emu::Bus bus;
	Emu::u8* mem = nullptr;

	void SetUp() override
	{
		cpu = bus.GetCpu();
		mem = bus.GetMemory();
		cpu.SetP(0);
		// clear reset cycles
		clearCycles(8);
	}

	void clearCycles(int x = 1)
	{
		for (int i = 0; i < x; ++i)
		{
			cpu.Step();
		}
	}
};


TEST_F(TestRegTransfer, TAX)
{
	A6502::ToyAssembler::Get().Assemble(R"(
		tax
		tax
	)", mem);

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
	A6502::ToyAssembler::Get().Assemble(R"(
		tay
		tay
	)", mem);

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
	A6502::ToyAssembler::Get().Assemble(R"(
		txa
		txa
	)", mem);

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
	A6502::ToyAssembler::Get().Assemble(R"(
		tya
		tya
	)", mem);

	cpu.SetY(0x80);

	clearCycles(2);

	ASSERT_EQ(cpu.A(), 0x80);          
	ASSERT_TRUE(cpu.P() & Emu::P_N_FLAG); 
                                          
	cpu.SetY(0);                      
                                          
	clearCycles(2);                       
	ASSERT_EQ(cpu.A(), 0);             
	ASSERT_TRUE(cpu.P() & Emu::P_Z_FLAG); 
}