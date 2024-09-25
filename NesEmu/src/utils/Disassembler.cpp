#include "pch.hpp"
#include "Disassembler.hpp"

namespace A6502
{
std::string FormatAddressingMode(AddressingModeName am, u16 val=0);
const char* GetInstructionName(InstructionName i);
const char* GetAddressingModeName(AddressingModeName a);

Disassembler& Disassembler::ConnectBus(Emu::Bus* bus)
{
	m_bus = bus;
	return *this;
}
Disassembly& Disassembler::Get(u16 addr)
{
	if (!m_cache.contains(addr))
	{
		DisassembleFromAddress(addr, true);
	}
	return m_cache.at(addr);
}
void Disassembler::Init(bool use_registers)
{
	m_cache.clear();
	m_label_counter = 0;

	const u16 reset = ( m_bus->Peek(0xFFFD) << 8 ) | m_bus->Peek(0xFFFC);
	const u16 nmi   = ( m_bus->Peek(0xFFFB) << 8 ) | m_bus->Peek(0xFFFA);
	const u16 irq   = ( m_bus->Peek(0xFFFF) << 8 ) | m_bus->Peek(0xFFFE);
	m_cache.insert({ reset, {"", "Reset"          } });
	m_cache.insert({ irq,   {"", "IRQ_subroutine" } });
	m_cache.insert({ nmi,   {"", "NMI_subroutine" } });


	DisassembleFromAddress(reset, use_registers);
	DisassembleFromAddress(irq, use_registers);
	DisassembleFromAddress(nmi, use_registers);
	std::fstream file("test.txt", std::ios::out);
	for (const auto& [k, v] : m_cache)
	{
		if (!v.label.empty())
		{
			file << v.label << ":\n";
		}
		file << std::hex << k << std::dec << " " << v.repr << '\n';
	}

}

std::map<u16, Disassembly>& Disassembler::GetCache()
{
	return m_cache;
}


void Disassembler::DisassembleFromAddress(size_t begin, bool use_registers)
{
	u16 caddr = static_cast<u16>( begin );

	u16 jmp_addr = caddr;
	u16 branch_addr = caddr;

	while (begin <= 0xFFFF)
	{
		if (m_cache.contains(caddr) && !m_cache.at(caddr).repr.empty())
		{
			return;
		}
		else
		{
			m_cache.insert({ caddr, {} });
		}
		const u8 instr = m_bus->Peek(caddr);
		const auto& opcode = s_instructions[instr];
		std::string str_rep = GetInstructionName(opcode.instruction);
		auto bytes = GetBytesForAddressingMode(opcode.mode);
		switch (bytes)
		{
		case 1:
		{
			str_rep += FormatAddressingMode(opcode.mode);
			break;
		}
		case 2:
		{
			if (IsConditionalJump(opcode))
			{
				i8 b0 = m_bus->Peek(( begin + 1 ) & 0xFFFF);
				u16 addr = ( begin + b0 + 2 ) & 0xFFFF;
				branch_addr = addr;
				str_rep += ' ';
				if (!m_cache.contains(addr))
				{
					m_cache.insert({ addr, {} });
				}
				if (m_cache[addr].label.empty())
				{
					std::string lbl = std::format("_label_{:04X}", m_label_counter++);
					str_rep += lbl;
					m_cache[addr].label = lbl;
				}
				else
				{
					str_rep += m_cache[addr].label;
				}
			}
			else
			{
				u8 b0 = m_bus->Peek(( caddr + 1 ) & 0xFFFF);
				str_rep += FormatAddressingMode(opcode.mode, b0);
			}
			break;
		}
		case 3:
		{
			u8 b0 = m_bus->Peek(( caddr + 1 ) & 0xFFFF);
			u8 b1 = m_bus->Peek(( caddr + 2 ) & 0xFFFF);
			u16 word = ( b1 << 8 ) | b0;
			jmp_addr = word;

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
		}

		m_cache[caddr].repr = str_rep;
		m_cache[caddr].size = bytes;
		m_cache[caddr].instruction = GetInstructionName(opcode.instruction);
		m_cache[caddr].addressing = GetAddressingModeName(opcode.mode);

		if (opcode.instruction == InstructionName::STP)
		{
			return;
		}
		if (s_is_jump[instr])
		{
			if (opcode.instruction == InstructionName::JMP && opcode.mode != AddressingModeName::IND)
			{
				DisassembleFromAddress(jmp_addr, use_registers);
				return;
			}
			else if (opcode.instruction == InstructionName::JSR)
			{
				DisassembleFromAddress(jmp_addr, use_registers);
			}
			else if (opcode.instruction == InstructionName::RTS)
			{
				return;
			}
			else if (opcode.instruction == InstructionName::BRK)
			{
				// no need to do anything, irq is disassembled already
			}
			else if (opcode.instruction == InstructionName::RTI)
			{
				return;
			}
			else if (IsConditionalJump(opcode))
			{
				DisassembleFromAddress(branch_addr, use_registers);
			}

		}

		begin += bytes;
		caddr = static_cast<u16>( begin );
	}

}


static std::string FormatAddressingMode(AddressingModeName am, u16 val /*=0*/)
{
	using A = A6502::AddressingModeName;
	switch (am)
	{
	case A::IMP: return "";
	case A::ACC: return " A";
	case A::IMM: return std::format(" #${:02X}", val);
	case A::IM2: return "";
	case A::ZPI: return std::format(" ${:02X}", val);
	case A::ZPX: return std::format(" ${:02X},X", val);
	case A::ZPY: return std::format(" ${:02X},Y", val);
	case A::REL: return std::format(" *${:+02X}", val); // will never happen
	case A::ABS: return std::format(" ${:04X}", val);
	case A::ABX: return std::format(" ${:04X},X", val);
	case A::ABY: return std::format(" ${:04X},Y", val);
	case A::IND: return std::format(" (${:04X})", val);
	case A::INX: return std::format(" (${:04X},X)", val);
	case A::INY: return std::format(" (${:04X}),Y", val);
	default: std::unreachable();
	}
}
const char* GetInstructionName(InstructionName i)
{
	using I = A6502::InstructionName;
	switch (i)
	{
	case I::ADC: return "ADC";
	case I::AND: return "AND";
	case I::ASL: return "ASL";
	case I::BCC: return "BCC";
	case I::BCS: return "BCS";
	case I::BEQ: return "BEQ";
	case I::BIT: return "BIT";
	case I::BMI: return "BMI";
	case I::BNE: return "BNE";
	case I::BPL: return "BPL";
	case I::BRK: return "BRK";
	case I::BVC: return "BVC";
	case I::BVS: return "BVS";
	case I::CLC: return "CLC";
	case I::CLD: return "CLD";
	case I::CLI: return "CLI";
	case I::CLV: return "CLV";
	case I::CMP: return "CMP";
	case I::CPX: return "CPX";
	case I::CPY: return "CPY";
	case I::DEC: return "DEC";
	case I::DEX: return "DEX";
	case I::DEY: return "DEY";
	case I::EOR: return "EOR";
	case I::INC: return "INC";
	case I::INX: return "INX";
	case I::INY: return "INY";
	case I::JMP: return "JMP";
	case I::JSR: return "JSR";
	case I::LDA: return "LDA";
	case I::LDX: return "LDX";
	case I::LDY: return "LDY";
	case I::LSR: return "LSR";
	case I::NOP: return "NOP";
	case I::ORA: return "ORA";
	case I::PHA: return "PHA";
	case I::PHP: return "PHP";
	case I::PLA: return "PLA";
	case I::PLP: return "PLP";
	case I::ROL: return "ROL";
	case I::ROR: return "ROR";
	case I::RTI: return "RTI";
	case I::RTS: return "RTS";
	case I::SBC: return "SBC";
	case I::SEC: return "SEC";
	case I::SED: return "SED";
	case I::SEI: return "SEI";
	case I::STA: return "STA";
	case I::STX: return "STX";
	case I::STY: return "STY";
	case I::TAX: return "TAX";
	case I::TAY: return "TAY";
	case I::TSX: return "TSX";
	case I::TXA: return "TXA";
	case I::TXS: return "TXS";
	case I::TYA: return "TYA";
	case I::ALR: return "ALR";
	case I::ANC: return "ANC";
	case I::ARR: return "ARR";
	case I::AXS: return "AXS";
	case I::LAX: return "LAX";
	case I::SAX: return "SAX";
	case I::DCP: return "DCP";
	case I::ISC: return "ISC";
	case I::RLA: return "RLA";
	case I::RRA: return "RRA";
	case I::SLO: return "SLO";
	case I::SRE: return "SRE";
	case I::DOP: return "DOP";
	case I::TOP: return "TOP";
	case I::STP: return "STP";
	case I::XAA: return "XAA";
	case I::AHX: return "AHX";
	case I::TAS: return "TAS";
	case I::SHY: return "SHY";
	case I::SHX: return "SHX";
	case I::LAS: return "LAS";

	default: throw std::runtime_error("Don't use that here please");

	}
}

const char* GetAddressingModeName(AddressingModeName a)
{
	using A = A6502::AddressingModeName;
	switch (a)
	{
	case A::IMP: return "IMP";
	case A::IM2: return "IMP";
	case A::ACC: return "ACC";
	case A::IMM: return "IMM";
	case A::ZPI: return "ZPI";
	case A::ZPX: return "ZPX";
	case A::ZPY: return "ZPY";
	case A::REL: return "REL";
	case A::ABS: return "ABS";
	case A::ABX: return "ABX";
	case A::ABY: return "ABY";
	case A::IND: return "IND";
	case A::INX: return "INX";
	case A::INY: return "INY";
	default: throw std::runtime_error("Don't use that here please");
	}

}
}
