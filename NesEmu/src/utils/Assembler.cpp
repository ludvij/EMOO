#include "pch.hpp"
#include "Assembler.hpp"

#include <ctre/ctre.hpp>

#include <print>

#include <lud_parser.hpp>


namespace A6502
{

static std::string upper(std::string s);

Assembler& Assembler::Assemble(const std::string& code)
{

	u16 write_pos = 0;

	// split lines
	for (const auto& line : ctre::split <"\n">(code))
	{
		std::string s = upper(line.str());
		if (auto m = ctre::search<"@RESET\\s+\\$([A-F0-9]{4})">(s))
		{
			u16 dat = Lud::parse_num<u16>(m.get<1>().view(), 16);
			u8 lo = dat & 0x00FF;
			u8 hi = ( dat & 0xFF00 ) >> 8;
			m_bus->Write(0xFFFC, lo);
			m_bus->Write(0xFFFD, hi);
		}
		else if (auto m = ctre::search<"@AT\\s+\\$([A-F0-9]{4})">(s))
		{
			write_pos = Lud::parse_num<u16>(m.get<1>().view(), 16);
		}
		else if (auto m = ctre::search<"([A-Z]{3})(\\s+([0-9A-FXY,\\-()#\\$]+))?">(s))
		{
			Opcode op = {};
			// parse line
			op.instruction = GetInstructionName(m.get<1>());
			auto [mode, dat] = ParseAddressingMode(m.get<3>());
			op.mode = mode;
			auto bytes = GetBytesForAddressingMode(mode);

			if (auto it = std::find(s_instructions.begin(), s_instructions.end(), op); it != s_instructions.end())
			{
				u8 index = static_cast<u8>( it - s_instructions.begin() );
				switch (bytes)
				{
				case 3:
				{
					u8 hi = ( dat & 0xFF00 ) >> 8;
					m_bus->Write(write_pos + 2, hi);
					[[fallthrough]];
				}
				case 2:
				{
					if (op.mode == AddressingModeName::REL)
					{
						i8 lo = static_cast<i8>( dat );
						m_bus->Write(write_pos + 1, lo);
					}
					else
					{
						u8 lo = dat & 0x00FF;
						m_bus->Write(write_pos + 1, lo);
					}
					[[fallthrough]];
				}
				case 1:
					m_bus->Write(write_pos, index);
				}
				write_pos += bytes;
			}
			else
			{
				std::println("{:s} this opcode does not exist, ignoring", s);
			}
		}
	}
	return *this;
}

void Assembler::Clean()
{
}

InstructionName Assembler::GetInstructionName(const std::string_view name)
{
	if (name == "ADC") return A6502::InstructionName::ADC;
	if (name == "AND") return A6502::InstructionName::AND;
	if (name == "ASL") return A6502::InstructionName::ASL;
	if (name == "BCC") return A6502::InstructionName::BCC;
	if (name == "BCS") return A6502::InstructionName::BCS;
	if (name == "BEQ") return A6502::InstructionName::BEQ;
	if (name == "BIT") return A6502::InstructionName::BIT;
	if (name == "BMI") return A6502::InstructionName::BMI;
	if (name == "BNE") return A6502::InstructionName::BNE;
	if (name == "BPL") return A6502::InstructionName::BPL;
	if (name == "BRK") return A6502::InstructionName::BRK;
	if (name == "BVC") return A6502::InstructionName::BVC;
	if (name == "BVS") return A6502::InstructionName::BVS;
	if (name == "CLC") return A6502::InstructionName::CLC;
	if (name == "CLD") return A6502::InstructionName::CLD;
	if (name == "CLI") return A6502::InstructionName::CLI;
	if (name == "CLV") return A6502::InstructionName::CLV;
	if (name == "CMP") return A6502::InstructionName::CMP;
	if (name == "CPX") return A6502::InstructionName::CPX;
	if (name == "CPY") return A6502::InstructionName::CPY;
	if (name == "DEC") return A6502::InstructionName::DEC;
	if (name == "DEX") return A6502::InstructionName::DEX;
	if (name == "DEY") return A6502::InstructionName::DEY;
	if (name == "EOR") return A6502::InstructionName::EOR;
	if (name == "INC") return A6502::InstructionName::INC;
	if (name == "INX") return A6502::InstructionName::INX;
	if (name == "INY") return A6502::InstructionName::INY;
	if (name == "JMP") return A6502::InstructionName::JMP;
	if (name == "JSR") return A6502::InstructionName::JSR;
	if (name == "LDA") return A6502::InstructionName::LDA;
	if (name == "LDX") return A6502::InstructionName::LDX;
	if (name == "LDY") return A6502::InstructionName::LDY;
	if (name == "LSR") return A6502::InstructionName::LSR;
	if (name == "NOP") return A6502::InstructionName::NOP;
	if (name == "ORA") return A6502::InstructionName::ORA;
	if (name == "PHA") return A6502::InstructionName::PHA;
	if (name == "PHP") return A6502::InstructionName::PHP;
	if (name == "PLA") return A6502::InstructionName::PLA;
	if (name == "PLP") return A6502::InstructionName::PLP;
	if (name == "ROL") return A6502::InstructionName::ROL;
	if (name == "ROR") return A6502::InstructionName::ROR;
	if (name == "RTI") return A6502::InstructionName::RTI;
	if (name == "RTS") return A6502::InstructionName::RTS;
	if (name == "SBC") return A6502::InstructionName::SBC;
	if (name == "SEC") return A6502::InstructionName::SEC;
	if (name == "SED") return A6502::InstructionName::SED;
	if (name == "SEI") return A6502::InstructionName::SEI;
	if (name == "STA") return A6502::InstructionName::STA;
	if (name == "STX") return A6502::InstructionName::STX;
	if (name == "STY") return A6502::InstructionName::STY;
	if (name == "TAX") return A6502::InstructionName::TAX;
	if (name == "TAY") return A6502::InstructionName::TAY;
	if (name == "TSX") return A6502::InstructionName::TSX;
	if (name == "TXA") return A6502::InstructionName::TXA;
	if (name == "TXS") return A6502::InstructionName::TXS;
	if (name == "TYA") return A6502::InstructionName::TYA;
	if (name == "ALR") return A6502::InstructionName::ALR;
	if (name == "ANC") return A6502::InstructionName::ANC;
	if (name == "ARR") return A6502::InstructionName::ARR;
	if (name == "AXS") return A6502::InstructionName::AXS;
	if (name == "LAX") return A6502::InstructionName::LAX;
	if (name == "SAX") return A6502::InstructionName::SAX;
	if (name == "DCP") return A6502::InstructionName::DCP;
	if (name == "ISC") return A6502::InstructionName::ISC;
	if (name == "RLA") return A6502::InstructionName::RLA;
	if (name == "RRA") return A6502::InstructionName::RRA;
	if (name == "SLO") return A6502::InstructionName::SLO;
	if (name == "SRE") return A6502::InstructionName::SRE;
	if (name == "DOP") return A6502::InstructionName::DOP;
	if (name == "TOP") return A6502::InstructionName::TOP;
	if (name == "STP") return A6502::InstructionName::STP;
	if (name == "XAA") return A6502::InstructionName::XAA;
	if (name == "AHX") return A6502::InstructionName::AHX;
	if (name == "TAS") return A6502::InstructionName::TAS;
	if (name == "SHY") return A6502::InstructionName::SHY;
	if (name == "SHX") return A6502::InstructionName::SHX;
	if (name == "LAS") return A6502::InstructionName::LAS;

	std::println("Instruction {:s} is unknow", name);
	return A6502::InstructionName::___;
}

AddressingModeData Assembler::ParseAddressingMode(const std::string_view mode)
{
	if (auto m = ctre::match<"">(mode))
	{
		return { A6502::AddressingModeName::IMP, 0 };
	}
	if (auto m = ctre::match<"A">(mode))
	{
		return { A6502::AddressingModeName::ACC, 0 };
	}
	if (auto m = ctre::match<"#\\$([A-F0-9]{2})">(mode))
	{
		auto dat = m.get<1>();
		return { A6502::AddressingModeName::IMM, Lud::parse_num<u16>(dat.view(), 16) };
	}
	if (auto m = ctre::match<"\\$([A-F0-9]{2})">(mode))
	{
		auto dat = m.get<1>();
		return { A6502::AddressingModeName::ZPI, Lud::parse_num<u16>(dat.view(), 16) };
	}
	if (auto m = ctre::match<"\\$([A-F0-9]{2}),X">(mode))
	{
		auto dat = m.get<1>();
		return { A6502::AddressingModeName::ZPX, Lud::parse_num<u16>(dat.view(), 16) };
	}
	if (auto m = ctre::match<"\\$([A-F0-9]{2}),Y">(mode))
	{
		auto dat = m.get<1>();
		return { A6502::AddressingModeName::ZPY, Lud::parse_num<u16>(dat.view(), 16) };
	}
	if (auto m = ctre::match<"(-?[0-9]{1,3})">(mode))
	{
		auto dat = m.get<1>();
		return { A6502::AddressingModeName::REL, static_cast<u16>( Lud::parse_num<i8>(dat.view(), 10) ) };
	}
	if (auto m = ctre::match<"\\$([A-F0-9]{4})">(mode))
	{
		auto dat = m.get<1>();
		return { A6502::AddressingModeName::ABS, Lud::parse_num<u16>(dat.view(), 16) };
	}
	if (auto m = ctre::match<"\\$([A-F0-9]{4}),X">(mode))
	{
		auto dat = m.get<1>();
		return { A6502::AddressingModeName::ABX, Lud::parse_num<u16>(dat.view(), 16) };
	}
	if (auto m = ctre::match<"\\$([A-F0-9]{4}),Y">(mode))
	{
		auto dat = m.get<1>();
		return { A6502::AddressingModeName::ABY, Lud::parse_num<u16>(dat.view(), 16) };
	}
	if (auto m = ctre::match<"\\(\\$([A-F0-9]{4})\\)">(mode))
	{
		auto dat = m.get<1>();
		return { A6502::AddressingModeName::IND, Lud::parse_num<u16>(dat.view(), 16) };
	}
	if (auto m = ctre::match<"\\(\\$([A-F0-9]{4}),X\\)">(mode))
	{
		auto dat = m.get<1>();
		return { A6502::AddressingModeName::INX, Lud::parse_num<u16>(dat.view(), 16) };
	}
	if (auto m = ctre::match<"\\(\\$([A-F0-9]{4})\\),Y">(mode))
	{
		auto dat = m.get<1>();
		return { A6502::AddressingModeName::INY, Lud::parse_num<u16>(dat.view(), 16) };
	}
	std::println("Unrecognized addressing mode {:s}", mode);
	return { A6502::AddressingModeName::___, 0 };
}

std::string upper(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c)
		{
			return std::toupper(c);
		});
	return s;
}

}