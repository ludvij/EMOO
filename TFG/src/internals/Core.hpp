#pragma once

#include <cstdint>

/*
* This file handles global definitions and defaults for the whole emulator
* It will also contain macros for logs and platform specific copde in case it's needed
*/

namespace Emu
{
/*
* Some type definitions to make the code easier to read
* since using uint8_t or unsigned char each time I want
* to use any internal it's going to be cumbersome.
* 
* I'll use Word basically for the PC and not much more.
* 
* u32 will be used for the frequency of the clocks
*/
using Byte = uint8_t;
using Word = uint16_t;
using u32 = uint32_t;

/*
* NTSC color subcarrier frequency
* Master clock is derived from it
* Even tho we are in a PAL region, emulating the NTSC system is better
* since the NES can run at 60 FPS instead of 50 FPS.
* Ideally we should be able to run PAL games in an NTSC system.
* 
* I'll keep the values of the PAL Color subcarrier frequency and 
* the clock divisor to make the nes run at the required speed just in case
*/
#if defined(NTSC_MODE) && !defined(PAL_MODE)
	constexpr u32 COLOR_SUBCARRIER_FREQ = 3579545;
	constexpr u32 CLOCK_DIVISOR = 12;
#elif defined(PAL_MODE) && !defined(NTSC_MODE)
	constexpr u32 COLOR_SUBCARRIER_FREQ = 4433619;
	constexpr u32 CLOCK_DIVISOR = 16;
#else
	#error Mode is not properly defined
#endif
/*
* Master clock of the nes
* it's 6 times the ntsc color subcarrier frequency
*/
constexpr u32 MASTER_CLOCK_SIGNAL = COLOR_SUBCARRIER_FREQ * 6;

}
