#include "pch.hpp"

#ifndef NES_EMU_TEST

#include <NesEmu.hpp>
#include <format>

#include <utils/Assembler.hpp>

std::ostream& operator<<(std::ostream& os, const Emu::CPU& c)
{
	os << std::format("PC:{:02x} INSTR:{:s} A:{:02x} X:{:02x} Y:{:02x} P:{:02x} SP:{:02x} CYC:{:d}", 
					  c.PC(), 
					  c.CurrentInstruction().name, 
					  c.A(),
					  c.X(),
					  c.Y(),
					  c.P(),
					  c.S(),
					  c.GetTotalCycles());
	return os;
}



int main()
{
	A6502::Assembler<Emu::Bus> a;
	Emu::Bus bus;

	a.Link(&bus);

	bus.GetCpu().Reset();
	

	a.Assemble(R"(
		ldx #23
		dex
		bne -3
		jmp $0000
	)");

	while(true)
	{
		std::cout << bus.GetCpu() << '\n';
		bus.GetCpu().Step();
	}


	return 0;
}

#endif