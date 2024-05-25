#include "pch.hpp"
#include "Disassembler.hpp"

namespace A6502
{
std::string FormatAddressingMode(AddressingModeName am, u16 val=0);
std::string GetInstructionName(InstructionName i);


Disassembler& Disassembler::ConnectBus(Emu::Bus* bus)
{
	m_bus = bus;
	return *this;
}
std::map<u16, Disassembly> Disassembler::Disassemble(bool use_registers)
{
	std::map<u16, Disassembly> result;

	u32 label_counter = 0;
	u32 fn_counter = 0;

	const u16 reset = ( m_bus->Peek(0xFFFD) << 8 ) | m_bus->Peek(0xFFFC);
	const u16 nmi   = ( m_bus->Peek(0xFFFB) << 8 ) | m_bus->Peek(0xFFFA);
	const u16 irq   = ( m_bus->Peek(0xFFFF) << 8 ) | m_bus->Peek(0xFFFE);
	result.insert({ reset, {"", "Reset"          } });
	result.insert({ irq,   {"", "IRQ subroutine" } });
	result.insert({ nmi,   {"", "NMI subroutine" } });
	u32 pc = 0x8000;
	while (pc <= 0xFFFF)
	{
		const auto& opcode = s_instructions[m_bus->Peek(pc)];
		std::string str_rep = GetInstructionName(opcode.instruction);
		auto bytes = GetBytesForAddressingMode(opcode.mode);

		if (!result.contains(pc))
		{
			result.insert({ pc, {} });
		}

		switch (bytes)
		{
		case 1:
		{
			str_rep += FormatAddressingMode(opcode.mode);
			break;
		}
		case 2:
		{
			if (opcode.mode == A6502::AddressingModeName::REL)
			{
				i8 b0 = m_bus->Peek(( pc + 1 ) & 0xFFFF);
				u16 addr = ( pc + b0 + 2 ) & 0xFFFF;
				str_rep += ' ';
				if (b0 > 0)
				{
					result.insert({ addr, {} });
				}
				if (result[addr].label.empty())
				{
					std::string lbl = std::format("_label_{:04X}", label_counter++);
					str_rep += lbl;
					result[addr].label = lbl;
				}
				else
				{
					str_rep += result[addr].label;
				}
			}
			else
			{
				u8 b0 = m_bus->Peek(( pc + 1 ) & 0xFFFF);
				str_rep += FormatAddressingMode(opcode.mode, b0);
			}
			break;
		}
		case 3:
		{
			u8 b0 = m_bus->Peek(( pc + 1 ) & 0xFFFF);
			u8 b1 = m_bus->Peek(( pc + 2 ) & 0xFFFF);
			u16 word = ( b1 << 8 ) | b0;
			if (use_registers && s_registers.contains(word))
			{
				str_rep += s_registers[word];
			}
			else
			{
				str_rep += FormatAddressingMode(opcode.mode, word);
			}

			break;
		}
		default: std::unreachable();
		}

		result[pc].repr = str_rep;
		// optmistic aproach
		pc += bytes;

	}

	return result;
}
static std::string FormatAddressingMode(AddressingModeName am, u16 val /*=0*/)
{
	switch (am)
	{
	case A6502::AddressingModeName::IMP: return "";
	case A6502::AddressingModeName::ACC: return " A";
	case A6502::AddressingModeName::IMM: return std::format(" #${:02X}", val);
	case A6502::AddressingModeName::ZPI: return std::format(" ${:02X}", val);
	case A6502::AddressingModeName::ZPX: return std::format(" ${:02X},X", val);
	case A6502::AddressingModeName::ZPY: return std::format(" ${:02X},Y", val);
	case A6502::AddressingModeName::REL: return std::format(" *${:+02X}", val); // will never happen
	case A6502::AddressingModeName::ABS: return std::format(" ${:04X}", val);
	case A6502::AddressingModeName::ABX: return std::format(" ${:04X},X", val);
	case A6502::AddressingModeName::ABY: return std::format(" ${:04X},Y", val);
	case A6502::AddressingModeName::IND: return std::format(" (${:04X})", val);
	case A6502::AddressingModeName::INX: return std::format(" (${:04X},X)", val);
	case A6502::AddressingModeName::INY: return std::format(" (${:04X}),Y", val);
	default: std::unreachable();
	}
}
std::string GetInstructionName(InstructionName i)
{
	switch (i)
	{
	case A6502::InstructionName::ADC: return "ADC";
	case A6502::InstructionName::AND: return "AND";
	case A6502::InstructionName::ASL: return "ASL";
	case A6502::InstructionName::BCC: return "BCC";
	case A6502::InstructionName::BCS: return "BCS";
	case A6502::InstructionName::BEQ: return "BEQ";
	case A6502::InstructionName::BIT: return "BIT";
	case A6502::InstructionName::BMI: return "BMI";
	case A6502::InstructionName::BNE: return "BNE";
	case A6502::InstructionName::BPL: return "BPL";
	case A6502::InstructionName::BRK: return "BRK";
	case A6502::InstructionName::BVC: return "BVC";
	case A6502::InstructionName::BVS: return "BVS";
	case A6502::InstructionName::CLC: return "CLC";
	case A6502::InstructionName::CLD: return "CLD";
	case A6502::InstructionName::CLI: return "CLI";
	case A6502::InstructionName::CLV: return "CLV";
	case A6502::InstructionName::CMP: return "CMP";
	case A6502::InstructionName::CPX: return "CPX";
	case A6502::InstructionName::CPY: return "CPY";
	case A6502::InstructionName::DEC: return "DEC";
	case A6502::InstructionName::DEX: return "DEX";
	case A6502::InstructionName::DEY: return "DEY";
	case A6502::InstructionName::EOR: return "EOR";
	case A6502::InstructionName::INC: return "INC";
	case A6502::InstructionName::INX: return "INX";
	case A6502::InstructionName::INY: return "INY";
	case A6502::InstructionName::JMP: return "JMP";
	case A6502::InstructionName::JSR: return "JSR";
	case A6502::InstructionName::LDA: return "LDA";
	case A6502::InstructionName::LDX: return "LDX";
	case A6502::InstructionName::LDY: return "LDY";
	case A6502::InstructionName::LSR: return "LSR";
	case A6502::InstructionName::NOP: return "NOP";
	case A6502::InstructionName::ORA: return "ORA";
	case A6502::InstructionName::PHA: return "PHA";
	case A6502::InstructionName::PHP: return "PHP";
	case A6502::InstructionName::PLA: return "PLA";
	case A6502::InstructionName::PLP: return "PLP";
	case A6502::InstructionName::ROL: return "ROL";
	case A6502::InstructionName::ROR: return "ROR";
	case A6502::InstructionName::RTI: return "RTI";
	case A6502::InstructionName::RTS: return "RTS";
	case A6502::InstructionName::SBC: return "SBC";
	case A6502::InstructionName::SEC: return "SEC";
	case A6502::InstructionName::SED: return "SED";
	case A6502::InstructionName::SEI: return "SEI";
	case A6502::InstructionName::STA: return "STA";
	case A6502::InstructionName::STX: return "STX";
	case A6502::InstructionName::STY: return "STY";
	case A6502::InstructionName::TAX: return "TAX";
	case A6502::InstructionName::TAY: return "TAY";
	case A6502::InstructionName::TSX: return "TSX";
	case A6502::InstructionName::TXA: return "TXA";
	case A6502::InstructionName::TXS: return "TXS";
	case A6502::InstructionName::TYA: return "TYA";
	case A6502::InstructionName::ALR: return "ALR";
	case A6502::InstructionName::ANC: return "ANC";
	case A6502::InstructionName::ARR: return "ARR";
	case A6502::InstructionName::AXS: return "AXS";
	case A6502::InstructionName::LAX: return "LAX";
	case A6502::InstructionName::SAX: return "SAX";
	case A6502::InstructionName::DCP: return "DCP";
	case A6502::InstructionName::ISC: return "ISC";
	case A6502::InstructionName::RLA: return "RLA";
	case A6502::InstructionName::RRA: return "RRA";
	case A6502::InstructionName::SLO: return "SLO";
	case A6502::InstructionName::SRE: return "SRE";
	case A6502::InstructionName::DOP: return "DOP";
	case A6502::InstructionName::TOP: return "TOP";
	case A6502::InstructionName::STP: return "STP";
	case A6502::InstructionName::XAA: return "XAA";
	case A6502::InstructionName::AHX: return "AHX";
	case A6502::InstructionName::TAS: return "TAS";
	case A6502::InstructionName::SHY: return "SHY";
	case A6502::InstructionName::SHX: return "SHX";
	case A6502::InstructionName::LAS: return "LAS";

	default: throw std::runtime_error("Don't use that here please");

	}
}
}
