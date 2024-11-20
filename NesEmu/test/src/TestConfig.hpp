#pragma once
#include <gtest/gtest.h>

#include <NesEmu.hpp>
#include <utils/Assembler.hpp>

#include "ROM/blank.hpp"
#include "ROM/nestest.hpp"
#include <lud_assert.hpp>

class TestFixture : public testing::Test
{
protected:
	Emu::Console console;
	A6502::Assembler asse;

	TestFixture()
		: console(Emu::NTSC)
	{
		asse.ConnectBus(&console.GetBus());
	}

	void SetUp() override
	{
		// reload Cartridge memory
		testing::internal::CaptureStdout();
		console.LoadCartridgeFromMemory(blank_nes, blank_nes_len);
		testing::internal::GetCapturedStdout();


		// force reset to get the values I want in the registers
		console.GetBus().Write(0xFFFC, 0);
		console.GetBus().Write(0xFFFB, 0);
		// clear startup cycles
		clearCycles(8);
	}

	void TearDown() override
	{
		testing::internal::CaptureStdout();
		console.UnloadCartridge();
		testing::internal::GetCapturedStdout();
	}

	void clearCycles(int x = 1)
	{
		for (int i = 0; i < x; ++i)
		{
			console.GetCpu().Step();
		}
	}
	void run_instruction(int n=1)
	{
		for (int i = 0; i < n; i++)
		{
			do
			{
				console.GetCpu().Step();
			} while (console.GetCpu().GetCycles() > 0);
		}
	}
};