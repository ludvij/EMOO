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
		throw std::exception("reading memory outside valid range");
	return memory[addr];
}

void Bus::Write(u16 addr, u8 val)
{
	if (addr >= RAM_SIZE) // commit sudoku
		throw std::exception("Writing memory outside valid range");
	memory[addr] = val;
}


}
