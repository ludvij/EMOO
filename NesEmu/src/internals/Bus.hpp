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

	void Step();

	CPU GetCpu() { return m_cpu; }

	u8* GetMemory() { return m_memory; }

private:
	// Bus components
	CPU m_cpu;

	u32 m_masterClock = 0;

	//memory 2kb
	u8 m_memory[RAM_SIZE] = {0};
};

}
