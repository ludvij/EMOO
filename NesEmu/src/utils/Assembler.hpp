#ifndef A6502_ASSEMBLER_HEADER
#define A6502_ASSEMBLER_HEADER

#include "A6502Core.hpp"

#include "internals/Bus.hpp"
#include <map>

#include <optional>
#include <string>

namespace A6502
{
struct AddressingModeData
{
	AddressingModeName name;
	u16 value;
};

class Assembler
{
public:
	Assembler& ConnectBus(Emu::Bus* bus)
	{
		m_bus = bus;
		return *this;
	}


	Assembler& Assemble(const std::string& code);

	void Clean();

	InstructionName GetInstructionName(const std::string_view name);
	AddressingModeData ParseAddressingMode(const std::string_view mode);


private:


private:

	Emu::Bus* m_bus;

};
}

#endif//A6502_ASSEMBLER_HEADER