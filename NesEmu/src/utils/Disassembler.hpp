#ifndef A6502_DISSASEMBLER_HEADER
#define A6502_DISSASEMBLER_HEADER

#include "internals/Core.hpp"
#include <map>
#include <string>
#include <vector>

#include "A6502Core.hpp"

namespace A6502
{



struct Disassembly
{
	std::string repr="";
	std::string label="";
};

class Disassembler
{
public:
	Disassembler& ConnectBus(Emu::Bus* bus);

	std::map<u16, Disassembly> Disassemble(bool use_registers=true);

private:
	Emu::Bus* m_bus{ nullptr };

	static inline std::unordered_map<u16, const char*> s_registers{
		{0x2000, " PPU_CONTROL"    }, {0x2001, " PPU_MASK"        }, {0x2002, " PPU_STATUS" }, {0x2003, " OAM_ADDRESS"},
		{0x2004, " OAM_DATA"       }, {0x2005, " PPU_SCROLL"      }, {0x2006, " PPU_ADDRESS"}, {0x2007, " PPU_DATA"   },
		{0x4000, " PULSE_1_VOLUME" }, {0x4001, " PULSE_1_SWEEP"   }, {0x4002, " PULSE_1_LO" }, {0x4003, " PULSE_1_HI" },
		{0x4004, " PULSE_2_VOLUME" }, {0x4005, " PULSE_2_SWEEP"   }, {0x4006, " PULSE_2_LO" }, {0x4007, " PULSE_2_HI" },
		{0x4008, " TRIANGLE_VOLUME"}, {0x4009, " TRIANGLE_SWEEP"  }, {0x400A, " TRIANGLE_LO"}, {0x400B, " TRIANGLE_HI"},
		{0x400C, " NOISE_VOLUME"   },                                {0x400E, " NOISE_LO"   }, {0x400F, " NOISE_HI"   },
		{0x4010, " DMC_FREQUENCY"  }, {0x4011, " DMC_LOAD_COUNTER"}, {0x4012, " DMC_START"  }, {0x4013, " DMC_LENGTH" },
		{0x4014, " OAM_DMA"        },
		{0x4015, " APU_STATUS"     },
		{0x4016, " JOYPAD_1"       }, {0x4017, " JOYPAD_2"        },
		// https://www.nesdev.org/wiki/CPU_Test_Mode
		{0x4018, " APU_TEST_1"     }, {0x4019, " APU_TEST_2"      }, {0x401A, " APU_TEST_3" },
		{0x401C, " CPU_TIMER_1"    }, {0x401D, " CPU_TIMER_2"     }, {0x401E, " CPU_TIMER_3"}, {0x401F, " CPU_TIMER_4"},
	};
};
}

#endif