#ifndef EMU_DISSASSEMBLER_HEADER
#define EMU_DISSASSEMBLER_HEADER


// fwd declaration to avoid circular dependencies
namespace Emu
{
class CPU;
}

namespace A6502
{
class Disassembler
{
public:
	Disassembler(Emu::CPU* cpu);

	std::string Disasemble();

private:
	std::string_view getAddrMode();

private:
	Emu::CPU* m_cpu;
};
}

#endif