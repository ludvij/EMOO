#pragma once

#include "Core.hpp"

#include "CPU.hpp"
#include <array>

namespace Emu
{
	
class Bus
{
public:
	Bus();

	u8 Read(u16 addr) const;
	void Write(u16 addr, u8 val);

	void Step();

	CPU& GetCpu() noexcept { return m_cpu; }


private:
	// Bus components
	CPU m_cpu;

	u32 m_masterClock = 0;

	//memory 2kb
	std::array<u8, RAM_SIZE> m_mem = {0};
};

}
