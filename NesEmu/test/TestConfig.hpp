#pragma once
#include <gtest/gtest.h>

#include <utils/Assembler.hpp>

class TestFixture : public testing::Test
{
protected:
	Emu::Bus bus;
	A6502::Assembler<Emu::Bus> asse;

	void SetUp() override
	{
		asse.Link(&bus);
		// force reset to get the values I want in the registers
		bus.GetCpu().Reset();
		clearCycles(8);
		// reset processor status
		bus.GetCpu().SetP(0);
	}

	void TearDown() override
	{
		asse.Clean();
	}

	void clearCycles(int x = 1)
	{
		for (int i = 0; i < x; ++i)
		{
			bus.GetCpu().Step();
		}
	}
};