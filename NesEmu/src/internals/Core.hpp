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
* Some type definitions to make the code easier to read
* since using uint8_t or unsigned char each time I want
* to use any internal it's going to be cumbersome.
*
* I'll use u16 basically for the m_PC and not much more.
*
* u32 will be used for the frequency of the clocks
*/
using i8  =  int8_t;
using u8  = uint8_t;
using i16 =  int16_t;
using u16 = uint16_t;
using i32 =  int32_t;
using u32 = uint32_t;
using i64 =  int64_t;
using u64 = uint64_t;

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


struct Configuration
{
	const u32 CpuClockDivisor;
	const u32 PpuClockDivisor;
	const u32 FrameRate;
	const float width;
	const float height;
	const char* palette_src;
};

inline constexpr Configuration NTSC{ 12, 4, 60, 256.0f, 240.0f, "palettes/NTSC.pal" };
inline constexpr Configuration PAL{ 16, 5, 50, 256.0f, 240.0f, "palettes/PAL.pal" };

/*
* Master clock of the nes
* it's 6 times the ntsc color subcarrier frequency
*/


}

#endif