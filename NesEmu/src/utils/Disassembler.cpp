#include "pch.hpp"
#include "Disassembler.hpp"

#include <internals/CPU.hpp>

#include <format>

void unreachable()
{
#if defined(NES_EMU_PLATFORM_WINDOWS)
	__assume(false);
#elif defined(NES_EMU_PLATFORM_LINUX)
	__builtin_unreachable();
#else
	#error Platform not defined
#endif
}

namespace A6502
{


Disassembler::Disassembler(Emu::CPU* cpu)
	: m_cpu(cpu)
{
	
}

std::string Disassembler::Disasemble()
{
	std::string_view addrMode = getAddrMode();
	std::string res = std::format("${:4X}: {S} {S}", 
								  m_cpu->m_PC, 
								  m_cpu->m_currentInstr.name, 
								  addrMode);

	return res;
}

std::string_view Disassembler::getAddrMode()
{
	auto addrModeNoString = m_cpu->m_currentInstr.addrMode;

	if (addrModeNoString == &Emu::CPU::addrIMM) return "IMM";
	if (addrModeNoString == &Emu::CPU::addrIMP) return "IMP";
	if (addrModeNoString == &Emu::CPU::addrABS) return "ABS";
	if (addrModeNoString == &Emu::CPU::addrABX) return "ABX";
	if (addrModeNoString == &Emu::CPU::addrABY) return "ABY";
	if (addrModeNoString == &Emu::CPU::addrZPI) return "ZPI";
	if (addrModeNoString == &Emu::CPU::addrZPX) return "ZPX";
	if (addrModeNoString == &Emu::CPU::addrZPY) return "ZPY";
	if (addrModeNoString == &Emu::CPU::addrIND) return "IND";
	if (addrModeNoString == &Emu::CPU::addrINX) return "INX";
	if (addrModeNoString == &Emu::CPU::addrINY) return "INY";
	if (addrModeNoString == &Emu::CPU::addrREL) return "REL";
	if (addrModeNoString == &Emu::CPU::addrACC) return "ACC";
}
}
