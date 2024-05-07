#pragma once
#include <gtest/gtest.h>

#include <NesEmu.hpp>
#include <utils/Assembler.hpp>

class TestFixture : public testing::Test
{
protected:
	Emu::Console console;
	A6502::Assembler<Emu::Bus> asse;

	TestFixture()
		: console(Emu::NTSC)
	{
		asse.Link(&console.GetBus());
	}

	void SetUp() override
	{
		// reload Cartridge memory
		console.LoadCartridge("blank.nes");


		// force reset to get the values I want in the registers
		console.Reset();
		console.GetBus().Write(0xFFFC, 0);
		console.GetBus().Write(0xFFFB, 0);
		// clear startup cycles
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
			console.GetCpu().Step();
		}
	}
};