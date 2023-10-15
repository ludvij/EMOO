#include <iostream>

#include <gtest/gtest.h>

#include <internals/Bus.hpp>
#include <internals/Core.hpp>



class TestFixture : public testing::Test, public Emu::Bus
{
protected:
	void SetUp() override
	{
	}

	void clearCycles(int x = 1)
	{
		for (int i = 0; i < x; ++i)
		{
			for (int i = 0; i < Emu::NTSC_CLOCK_DIVISOR; ++i) 
			{
				Step();
			}
		}
	}


};


TEST_F(TestFixture, ADC)
{
	// ADC IMM
	Write(0, 0x69);
	Write(1, 0);
	// clock the system 12 times
	Step();
	ASSERT_EQ(m_cpu.A(), 0);
	// x is zero 
	ASSERT_TRUE(m_cpu.P() & Emu::P_Z_FLAG);

	// ADD ZPI
	Write(2, 0x65);
	Write(3, 4);
	Write(4, 0x80);

	clearCycles(2);
	// setting negative flag
	ASSERT_EQ(m_cpu.A(), 0x80);
	ASSERT_TRUE(m_cpu.P() & Emu::P_N_FLAG);

	// ADC ZPX
	Write(4, 0x75);
	Write(5, 6);
	Write(6, 0x80);

	clearCycles(4);

	// setting z and c flags in overflow
	ASSERT_EQ(m_cpu.A(), 0x0);
	ASSERT_TRUE(m_cpu.P() & (Emu::P_Z_FLAG | Emu::P_C_FLAG));

	// ADD ABS
	Write(6, 0x6D);
	Write(7, 9);
	Write(8, 0);
	Write(9, 1);

	clearCycles(4);

	// checking C flag is added
	ASSERT_EQ(m_cpu.A(), 2);

	// TODO: test V flag
	// TODO: test ABX mode
	// TODO: test ABY mode
	// TODO: test INX mode
	// TODO: test INY mode
	// TODO: check for oopscycles
	
}






int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}