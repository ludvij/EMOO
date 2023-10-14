#pragma once

#include "Core.hpp"

#include "CPU.hpp"

namespace Emu
{
	
class Bus
{
public:
	Bus();

	u8 Read(u16 addr);
	void Write(u16 addr, u8 val);

private:
	// Bus components
	CPU* cpu;
};

}
