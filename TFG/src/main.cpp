#include "pch.hpp"

#include "internals/Bus.hpp"
#include "internals/CPU.hpp"

int main()
{
	std::cout << "TFG WIP" << std::endl;

	Emu::Bus bus;
	Emu::CPU cpu;
	cpu.ConnectBus(&bus);

	return 0;
}