#ifndef EMU_CORE_HEADER
#define EMU_CORE_HEADER

#include <cstdint>

/*
* This file handles global definitions and defaults for the whole emulator
* It will also contain macros for logs and platform specific copde in case it's needed
*/



namespace Emu
{
/*
* Some type definitions to make the code easier to readMemory
* since using uint8_t or unsigned char each time I want
* to use any internal it's going to be cumbersome.
* 
* I'll use u16 basically for the m_PC and not much more.
* 
* u32 will be used for the frequency of the clocks
*/
using s8  =  int8_t;
using u8  = uint8_t;
using s16 =  int16_t;
using u16 = uint16_t;
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
constexpr u32 NTSC_COLOR_SUBCARRIER_FREQ = 3579545;
constexpr u32 NTSC_CLOCK_DIVISOR = 12;
constexpr u32  PAL_COLOR_SUBCARRIER_FREQ = 4433619;
constexpr u32  PAL_CLOCK_DIVISOR = 16;
/*
* Master clock of the nes
* it's 6 times the ntsc color subcarrier frequency
*/
constexpr u32 NTSC_MASTER_CLOCK_SIGNAL = NTSC_COLOR_SUBCARRIER_FREQ * 6;
constexpr u32  PAL_MASTER_CLOCK_SIGNAL =  PAL_COLOR_SUBCARRIER_FREQ * 6;


/*
* ram sizes and stuff
*/
constexpr u16 RAM_SIZE = 0x0800;

}

#endif