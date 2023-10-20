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

	bus.GetCpu().SetA(0x80);

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().X(), 0x80);          
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_N_FLAG); 
                                          
	bus.GetCpu().SetA(0);                      
                                          
	clearCycles(2);                       
	ASSERT_EQ(bus.GetCpu().X(), 0);             
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_Z_FLAG); 
}

TEST_F(TestRegTransfer, TAY)
{
	asse.Assemble(R"(
		tay
		tay
	)");

	bus.GetCpu().SetA(0x80);

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().Y(), 0x80);          
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_N_FLAG); 
                                          
	bus.GetCpu().SetA(0);                      
                                          
	clearCycles(2);                       
	ASSERT_EQ(bus.GetCpu().Y(), 0);             
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_Z_FLAG); 
}

TEST_F(TestRegTransfer, TXA)
{
	asse.Assemble(R"(
		txa
		txa
	)");

	bus.GetCpu().SetX(0x80);

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().A(), 0x80);          
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_N_FLAG); 
                                          
	bus.GetCpu().SetX(0);                      
                                          
	clearCycles(2);                       
	ASSERT_EQ(bus.GetCpu().A(), 0);             
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_Z_FLAG); 
}

TEST_F(TestRegTransfer, TYA)
{
	asse.Assemble(R"(
		tya
		tya
	)");

	bus.GetCpu().SetY(0x80);

	clearCycles(2);

	ASSERT_EQ(bus.GetCpu().A(), 0x80);          
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_N_FLAG); 
                                          
	bus.GetCpu().SetY(0);                      
                                          
	clearCycles(2);                       
	ASSERT_EQ(bus.GetCpu().A(), 0);             
	ASSERT_TRUE(bus.GetCpu().P() & Emu::P_Z_FLAG); 
}