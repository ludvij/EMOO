#include "pch.hpp"
#include "Bus.hpp"

namespace Emu
{
Bus::Bus()
{
	cpu.ConnectBus(this);
}

u8 Bus::Read(u16 addr)
{
	if (addr >= RAM_SIZE) // commit sudoku
		std::exit(0);
	return memory[addr];
}

void Bus::Write(u16 addr, u8 val)
{
	if (addr >= RAM_SIZE) // commit sudoku
		std::exit(0);
	memory[addr] = val;
}


}
