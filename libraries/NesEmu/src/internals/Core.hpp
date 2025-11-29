#ifndef EMU_CORE_HEADER
#define EMU_CORE_HEADER

#include <cstdint>

#include <chrono>
using namespace std::chrono_literals;

/*
* This file handles global definitions and defaults for the whole emulator
* It will also contain macros for logs and platform specific copde in case it's needed
*/

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


namespace Emu
{


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
	u32 cpu_clock_divisor;
	u32 ppu_clock_divisor;
	u32 frame_rate;
	std::chrono::microseconds frame_time;
	float width;
	float height;
};

inline constexpr Configuration NTSC{ 12, 4, 60, 16667us,  256.0f, 240.0f };
inline constexpr Configuration PAL{ 16, 5, 50, 20000us, 256.0f, 240.0f };

/*
* Master clock of the nes
* it's 6 times the ntsc color subcarrier frequency
*/


}

#endif