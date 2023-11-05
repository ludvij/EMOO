#pragma once
#include <gtest/gtest.h>

#include <NesEmu.hpp>

#include <utils/Assembler.hpp>

class TestFixture : public testing::Test
{
protected:
	Emu::CPU cpu;
	Emu::Bus memory;
	A6502::Assembler<Emu::Bus> asse;

	void SetUp() override
	{
		asse.Link(&memory);
		cpu.ConnectBus(&memory);
		// force reset to get the values I want in the registers
		cpu.Reset();
		memory.Write(0xFFFC, 0);
		memory.Write(0xFFFB, 0);
		clearCycles(8);
	}

	void TearDown() override
	{
		asse.Clean();
	}

	void clearCycles(int x = 1)
	{
		for (int i = 0; i < x; ++i)
		{
			cpu.Step();
		}
	}
};