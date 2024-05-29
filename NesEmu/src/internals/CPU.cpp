#include "pch.hpp"

#include "CPU.hpp"

#include "Bus.hpp"



namespace Emu
{



constexpr auto MAKE_WORD = [](const u16 hi, const u16 lo) -> u16
	{
		return static_cast<u16>( ( hi << 8 ) ) | lo;
	};
CPU::~CPU()
{

}
CPU::CPU()
{
	// filling the jump table
	Instruction invalid = { "STP", &CPU::addrIMM, &CPU::STP, 0 };
	std::ranges::fill(m_jumpTable, invalid);
	// ADC
	m_jumpTable[0x69] = { "ADC", &CPU::addrIMM, &CPU::ADC, 2 };
	m_jumpTable[0x65] = { "ADC", &CPU::addrZPI, &CPU::ADC, 3 };
	m_jumpTable[0x75] = { "ADC", &CPU::addrZPX, &CPU::ADC, 4 };
	m_jumpTable[0x6D] = { "ADC", &CPU::addrABS, &CPU::ADC, 4 };
	m_jumpTable[0x7D] = { "ADC", &CPU::addrABX, &CPU::ADC, 4 };
	m_jumpTable[0x79] = { "ADC", &CPU::addrABY, &CPU::ADC, 4 };
	m_jumpTable[0x61] = { "ADC", &CPU::addrINX, &CPU::ADC, 6 };
	m_jumpTable[0x71] = { "ADC", &CPU::addrINY, &CPU::ADC, 5 };
	// AND
	m_jumpTable[0x29] = { "AND", &CPU::addrIMM, &CPU::AND, 2 };
	m_jumpTable[0x25] = { "AND", &CPU::addrZPI, &CPU::AND, 3 };
	m_jumpTable[0x35] = { "AND", &CPU::addrZPX, &CPU::AND, 4 };
	m_jumpTable[0x2D] = { "AND", &CPU::addrABS, &CPU::AND, 4 };
	m_jumpTable[0x3D] = { "AND", &CPU::addrABX, &CPU::AND, 4 };
	m_jumpTable[0x39] = { "AND", &CPU::addrABY, &CPU::AND, 4 };
	m_jumpTable[0x21] = { "AND", &CPU::addrINX, &CPU::AND, 6 };
	m_jumpTable[0x31] = { "AND", &CPU::addrINY, &CPU::AND, 5 };
	// ASL
	m_jumpTable[0x0A] = { "ASL", &CPU::addrACC, &CPU::ASL, 2 };
	m_jumpTable[0x06] = { "ASL", &CPU::addrZPI, &CPU::ASL, 5 };
	m_jumpTable[0x0E] = { "ASL", &CPU::addrABS, &CPU::ASL, 6 };
	m_jumpTable[0x16] = { "ASL", &CPU::addrZPX, &CPU::ASL, 6 };
	m_jumpTable[0x1E] = { "ASL", &CPU::addrABX, &CPU::ASL, 7 };
	// BCC
	m_jumpTable[0x90] = { "BCC", &CPU::addrREL, &CPU::BCC, 2 };
	// BCS
	m_jumpTable[0xB0] = { "BCS", &CPU::addrREL, &CPU::BCS, 2 };
	// BEQ
	m_jumpTable[0xF0] = { "BEQ", &CPU::addrREL, &CPU::BEQ, 2 };
	// BIT
	m_jumpTable[0x24] = { "BIT", &CPU::addrZPI, &CPU::BIT, 3 };
	m_jumpTable[0x2C] = { "BIT", &CPU::addrABS, &CPU::BIT, 4 };
	// BMI
	m_jumpTable[0x30] = { "BMI", &CPU::addrREL, &CPU::BMI, 2 };
	// BNE
	m_jumpTable[0xD0] = { "BNE", &CPU::addrREL, &CPU::BNE, 2 };
	// BPL
	m_jumpTable[0x10] = { "BPL", &CPU::addrREL, &CPU::BPL, 2 };
	// BRK
	m_jumpTable[0x00] = { "BRK", &CPU::addrIMP, &CPU::BRK, 7 };
	// BVC
	m_jumpTable[0x50] = { "BVC", &CPU::addrREL, &CPU::BVC, 2 };
	// BVS
	m_jumpTable[0x70] = { "BVS", &CPU::addrREL, &CPU::BVS, 2 };
	//CLC
	m_jumpTable[0x18] = { "CLC", &CPU::addrIMP, &CPU::CLC, 2 };
	//CLD
	m_jumpTable[0xD8] = { "CLD", &CPU::addrIMP, &CPU::CLD, 2 };
	//CLI
	m_jumpTable[0x58] = { "CLI", &CPU::addrIMP, &CPU::CLI, 2 };
	//CLV
	m_jumpTable[0xB8] = { "CLV", &CPU::addrIMP, &CPU::CLV, 2 };
	//CMP
	m_jumpTable[0xC9] = { "CMP", &CPU::addrIMM, &CPU::CMP, 2 };
	m_jumpTable[0xC5] = { "CMP", &CPU::addrZPI, &CPU::CMP, 3 };
	m_jumpTable[0xD5] = { "CMP", &CPU::addrZPX, &CPU::CMP, 4 };
	m_jumpTable[0xCD] = { "CMP", &CPU::addrABS, &CPU::CMP, 4 };
	m_jumpTable[0xDD] = { "CMP", &CPU::addrABX, &CPU::CMP, 4 };
	m_jumpTable[0xD9] = { "CMP", &CPU::addrABY, &CPU::CMP, 4 };
	m_jumpTable[0xC1] = { "CMP", &CPU::addrINX, &CPU::CMP, 6 };
	m_jumpTable[0xD1] = { "CMP", &CPU::addrINY, &CPU::CMP, 5 };
	//CPX
	m_jumpTable[0xE0] = { "CPX", &CPU::addrIMM, &CPU::CPX, 2 };
	m_jumpTable[0xE4] = { "CPX", &CPU::addrZPI, &CPU::CPX, 3 };
	m_jumpTable[0xEC] = { "CPX", &CPU::addrABS, &CPU::CPX, 4 };
	//CPY
	m_jumpTable[0xC0] = { "CPY", &CPU::addrIMM, &CPU::CPY, 2 };
	m_jumpTable[0xC4] = { "CPY", &CPU::addrZPI, &CPU::CPY, 3 };
	m_jumpTable[0xCC] = { "CPY", &CPU::addrABS, &CPU::CPY, 4 };
	//DEC
	m_jumpTable[0xC6] = { "DEC", &CPU::addrZPI, &CPU::DEC, 5 };
	m_jumpTable[0xD6] = { "DEC", &CPU::addrZPX, &CPU::DEC, 6 };
	m_jumpTable[0xCE] = { "DEC", &CPU::addrABS, &CPU::DEC, 6 };
	m_jumpTable[0xDE] = { "DEC", &CPU::addrABX, &CPU::DEC, 7 };
	//DEX
	m_jumpTable[0xCA] = { "DEX", &CPU::addrIMP, &CPU::DEX, 2 };
	//DEY
	m_jumpTable[0x88] = { "DEY", &CPU::addrIMP, &CPU::DEY, 2 };
	//EOR
	m_jumpTable[0x49] = { "EOR", &CPU::addrIMM, &CPU::EOR, 2 };
	m_jumpTable[0x45] = { "EOR", &CPU::addrZPI, &CPU::EOR, 3 };
	m_jumpTable[0x55] = { "EOR", &CPU::addrZPX, &CPU::EOR, 4 };
	m_jumpTable[0x4D] = { "EOR", &CPU::addrABS, &CPU::EOR, 4 };
	m_jumpTable[0x5D] = { "EOR", &CPU::addrABX, &CPU::EOR, 4 };
	m_jumpTable[0x59] = { "EOR", &CPU::addrABY, &CPU::EOR, 4 };
	m_jumpTable[0x41] = { "EOR", &CPU::addrINX, &CPU::EOR, 6 };
	m_jumpTable[0x51] = { "EOR", &CPU::addrINY, &CPU::EOR, 5 };
	//INC
	m_jumpTable[0xE6] = { "INC", &CPU::addrZPI, &CPU::INC, 5 };
	m_jumpTable[0xF6] = { "INC", &CPU::addrZPX, &CPU::INC, 6 };
	m_jumpTable[0xEE] = { "INC", &CPU::addrABS, &CPU::INC, 6 };
	m_jumpTable[0xFE] = { "INC", &CPU::addrABX, &CPU::INC, 7 };
	//INX
	m_jumpTable[0xE8] = { "INX", &CPU::addrIMP, &CPU::INX, 2 };
	//INY
	m_jumpTable[0xC8] = { "INY", &CPU::addrIMP, &CPU::INY, 2 };
	//JMP
	m_jumpTable[0x4C] = { "JMP", &CPU::addrABS, &CPU::JMP, 3 };
	m_jumpTable[0x6C] = { "JMP", &CPU::addrIND, &CPU::JMP, 5 };
	//JSR
	m_jumpTable[0x20] = { "JSR", &CPU::addrABS, &CPU::JSR, 6 };
	//LDA
	m_jumpTable[0xA9] = { "LDA", &CPU::addrIMM, &CPU::LDA, 2 };
	m_jumpTable[0xA5] = { "LDA", &CPU::addrZPI, &CPU::LDA, 3 };
	m_jumpTable[0xB5] = { "LDA", &CPU::addrZPX, &CPU::LDA, 4 };
	m_jumpTable[0xAD] = { "LDA", &CPU::addrABS, &CPU::LDA, 4 };
	m_jumpTable[0xBD] = { "LDA", &CPU::addrABX, &CPU::LDA, 4 };
	m_jumpTable[0xB9] = { "LDA", &CPU::addrABY, &CPU::LDA, 4 };
	m_jumpTable[0xA1] = { "LDA", &CPU::addrINX, &CPU::LDA, 6 };
	m_jumpTable[0xB1] = { "LDA", &CPU::addrINY, &CPU::LDA, 5 };
	//LDX
	m_jumpTable[0xA2] = { "LDX", &CPU::addrIMM, &CPU::LDX, 2 };
	m_jumpTable[0xA6] = { "LDX", &CPU::addrZPI, &CPU::LDX, 3 };
	m_jumpTable[0xB6] = { "LDX", &CPU::addrZPY, &CPU::LDX, 4 };
	m_jumpTable[0xAE] = { "LDX", &CPU::addrABS, &CPU::LDX, 4 };
	m_jumpTable[0xBE] = { "LDX", &CPU::addrABY, &CPU::LDX, 4 };
	//LDY
	m_jumpTable[0xA0] = { "LDY", &CPU::addrIMM, &CPU::LDY, 2 };
	m_jumpTable[0xA4] = { "LDY", &CPU::addrZPI, &CPU::LDY, 3 };
	m_jumpTable[0xB4] = { "LDY", &CPU::addrZPX, &CPU::LDY, 4 };
	m_jumpTable[0xAC] = { "LDY", &CPU::addrABS, &CPU::LDY, 4 };
	m_jumpTable[0xBC] = { "LDY", &CPU::addrABX, &CPU::LDY, 4 };
	//LSR
	m_jumpTable[0x4A] = { "LSR", &CPU::addrACC, &CPU::LSR, 2 };
	m_jumpTable[0x46] = { "LSR", &CPU::addrZPI, &CPU::LSR, 5 };
	m_jumpTable[0x56] = { "LSR", &CPU::addrZPX, &CPU::LSR, 6 };
	m_jumpTable[0x4E] = { "LSR", &CPU::addrABS, &CPU::LSR, 6 };
	m_jumpTable[0x5E] = { "LSR", &CPU::addrABX, &CPU::LSR, 7 };
	//NOP
	m_jumpTable[0xEA] = { "NOP", &CPU::addrIMP, &CPU::NOP, 2 };
	//ORA
	m_jumpTable[0x09] = { "ORA", &CPU::addrIMM, &CPU::ORA, 2 };
	m_jumpTable[0x05] = { "ORA", &CPU::addrZPI, &CPU::ORA, 3 };
	m_jumpTable[0x15] = { "ORA", &CPU::addrZPX, &CPU::ORA, 4 };
	m_jumpTable[0x0D] = { "ORA", &CPU::addrABS, &CPU::ORA, 4 };
	m_jumpTable[0x1D] = { "ORA", &CPU::addrABX, &CPU::ORA, 4 };
	m_jumpTable[0x19] = { "ORA", &CPU::addrABY, &CPU::ORA, 4 };
	m_jumpTable[0x01] = { "ORA", &CPU::addrINX, &CPU::ORA, 6 };
	m_jumpTable[0x11] = { "ORA", &CPU::addrINY, &CPU::ORA, 5 };
	//PHA
	m_jumpTable[0x48] = { "PHA", &CPU::addrIMP, &CPU::PHA, 3 };
	//PHP
	m_jumpTable[0x08] = { "PHP", &CPU::addrIMP, &CPU::PHP, 3 };
	//PLA
	m_jumpTable[0x68] = { "PLA", &CPU::addrIMP, &CPU::PLA, 4 };
	//PLP
	m_jumpTable[0x28] = { "PLP", &CPU::addrIMP, &CPU::PLP, 4 };
	//ROL
	m_jumpTable[0x2A] = { "ROL", &CPU::addrACC, &CPU::ROL, 2 };
	m_jumpTable[0x26] = { "ROL", &CPU::addrZPI, &CPU::ROL, 5 };
	m_jumpTable[0x36] = { "ROL", &CPU::addrZPX, &CPU::ROL, 6 };
	m_jumpTable[0x2E] = { "ROL", &CPU::addrABS, &CPU::ROL, 6 };
	m_jumpTable[0x3E] = { "ROL", &CPU::addrABX, &CPU::ROL, 7 };
	//ROR
	m_jumpTable[0x6A] = { "ROR", &CPU::addrACC, &CPU::ROR, 2 };
	m_jumpTable[0x66] = { "ROR", &CPU::addrZPI, &CPU::ROR, 5 };
	m_jumpTable[0x76] = { "ROR", &CPU::addrZPX, &CPU::ROR, 6 };
	m_jumpTable[0x6E] = { "ROR", &CPU::addrABS, &CPU::ROR, 6 };
	m_jumpTable[0x7E] = { "ROR", &CPU::addrABX, &CPU::ROR, 7 };
	// RTI
	m_jumpTable[0x40] = { "RTI", &CPU::addrIMP, &CPU::RTI, 6 };
	//RTS
	m_jumpTable[0x60] = { "RTS", &CPU::addrIMP, &CPU::RTS, 6 };
	//SBC
	m_jumpTable[0xE9] = { "SBC", &CPU::addrIMM, &CPU::SBC, 2 };
	m_jumpTable[0xE5] = { "SBC", &CPU::addrZPI, &CPU::SBC, 3 };
	m_jumpTable[0xF5] = { "SBC", &CPU::addrZPX, &CPU::SBC, 4 };
	m_jumpTable[0xED] = { "SBC", &CPU::addrABS, &CPU::SBC, 4 };
	m_jumpTable[0xFD] = { "SBC", &CPU::addrABX, &CPU::SBC, 4 };
	m_jumpTable[0xF9] = { "SBC", &CPU::addrABY, &CPU::SBC, 4 };
	m_jumpTable[0xE1] = { "SBC", &CPU::addrINX, &CPU::SBC, 6 };
	m_jumpTable[0xF1] = { "SBC", &CPU::addrINY, &CPU::SBC, 5 };
	//SEC
	m_jumpTable[0x38] = { "SEC", &CPU::addrIMP, &CPU::SEC, 2 };
	//SED
	m_jumpTable[0xF8] = { "SED", &CPU::addrIMP, &CPU::SED, 2 };
	//SEI
	m_jumpTable[0x78] = { "SEI", &CPU::addrIMP, &CPU::SEI, 2 };
	//STA
	m_jumpTable[0x85] = { "STA", &CPU::addrZPI, &CPU::STA, 3 };
	m_jumpTable[0x95] = { "STA", &CPU::addrZPX, &CPU::STA, 4 };
	m_jumpTable[0x8D] = { "STA", &CPU::addrABS, &CPU::STA, 4 };
	m_jumpTable[0x9D] = { "STA", &CPU::addrABX, &CPU::STA, 4 };
	m_jumpTable[0x99] = { "STA", &CPU::addrABY, &CPU::STA, 5 };
	m_jumpTable[0x81] = { "STA", &CPU::addrINX, &CPU::STA, 6 };
	m_jumpTable[0x91] = { "STA", &CPU::addrINY, &CPU::STA, 6 };
	//STX
	m_jumpTable[0x86] = { "STX", &CPU::addrZPI, &CPU::STX, 3 };
	m_jumpTable[0x96] = { "STX", &CPU::addrZPY, &CPU::STX, 4 };
	m_jumpTable[0x8E] = { "STX", &CPU::addrABS, &CPU::STX, 4 };
	//STY
	m_jumpTable[0x84] = { "STY", &CPU::addrZPI, &CPU::STY, 3 };
	m_jumpTable[0x94] = { "STY", &CPU::addrZPX, &CPU::STY, 4 };
	m_jumpTable[0x8C] = { "STY", &CPU::addrABS, &CPU::STY, 4 };
	//TAX
	m_jumpTable[0xAA] = { "TAX", &CPU::addrIMP, &CPU::TAX, 2 };
	//TAY
	m_jumpTable[0xA8] = { "TAY", &CPU::addrIMP, &CPU::TAY, 2 };
	//TSX
	m_jumpTable[0xBA] = { "TSX", &CPU::addrIMP, &CPU::TSX, 2 };
	//TXA
	m_jumpTable[0x8A] = { "TXA", &CPU::addrIMP, &CPU::TXA, 2 };
	//TXS
	m_jumpTable[0x9A] = { "TXS", &CPU::addrIMP, &CPU::TXS, 2 };
	//TYA
	m_jumpTable[0x98] = { "TYA", &CPU::addrIMP, &CPU::TYA, 2 };

	// illegal opcodes
	// NOPs
	// NOP
	m_jumpTable[0x1A] = { "*NOP", &CPU::addrIMP, &CPU::NOP, 2 };
	m_jumpTable[0x3A] = { "*NOP", &CPU::addrIMP, &CPU::NOP, 2 };
	m_jumpTable[0x5A] = { "*NOP", &CPU::addrIMP, &CPU::NOP, 2 };
	m_jumpTable[0x7A] = { "*NOP", &CPU::addrIMP, &CPU::NOP, 2 };
	m_jumpTable[0xDA] = { "*NOP", &CPU::addrIMP, &CPU::NOP, 2 };
	m_jumpTable[0xFA] = { "*NOP", &CPU::addrIMP, &CPU::NOP, 2 };
	// SKB
	m_jumpTable[0x80] = { "*NOP", &CPU::addrIMM, &CPU::SKB, 2 };
	m_jumpTable[0x82] = { "*NOP", &CPU::addrIMM, &CPU::SKB, 2 };
	m_jumpTable[0x89] = { "*NOP", &CPU::addrIMM, &CPU::SKB, 2 };
	m_jumpTable[0xC2] = { "*NOP", &CPU::addrIMM, &CPU::SKB, 2 };
	m_jumpTable[0xE2] = { "*NOP", &CPU::addrIMM, &CPU::SKB, 2 };
	// IGN
	m_jumpTable[0x0C] = { "*NOP", &CPU::addrABS, &CPU::IGN, 4 };
	m_jumpTable[0x1C] = { "*NOP", &CPU::addrABX, &CPU::IGN, 4 };
	m_jumpTable[0x3C] = { "*NOP", &CPU::addrABX, &CPU::IGN, 4 };
	m_jumpTable[0x5C] = { "*NOP", &CPU::addrABX, &CPU::IGN, 4 };
	m_jumpTable[0x7C] = { "*NOP", &CPU::addrABX, &CPU::IGN, 4 };
	m_jumpTable[0xDC] = { "*NOP", &CPU::addrABX, &CPU::IGN, 4 };
	m_jumpTable[0xFC] = { "*NOP", &CPU::addrABX, &CPU::IGN, 4 };
	m_jumpTable[0x04] = { "*NOP", &CPU::addrZPI, &CPU::IGN, 3 };
	m_jumpTable[0x44] = { "*NOP", &CPU::addrZPI, &CPU::IGN, 3 };
	m_jumpTable[0x64] = { "*NOP", &CPU::addrZPI, &CPU::IGN, 3 };
	m_jumpTable[0x14] = { "*NOP", &CPU::addrZPX, &CPU::IGN, 4 };
	m_jumpTable[0x34] = { "*NOP", &CPU::addrZPX, &CPU::IGN, 4 };
	m_jumpTable[0x54] = { "*NOP", &CPU::addrZPX, &CPU::IGN, 4 };
	m_jumpTable[0x74] = { "*NOP", &CPU::addrZPX, &CPU::IGN, 4 };
	m_jumpTable[0xD4] = { "*NOP", &CPU::addrZPX, &CPU::IGN, 4 };
	m_jumpTable[0xF4] = { "*NOP", &CPU::addrZPX, &CPU::IGN, 4 };

	// Combined operations
	// ALR
	m_jumpTable[0x4B] = { "*ALR", &CPU::addrIMM, &CPU::ALR, 2 };
	// ANC
	m_jumpTable[0x0B] = { "*ANC", &CPU::addrIMM, &CPU::ANC, 2 };
	m_jumpTable[0x2B] = { "*ANC", &CPU::addrIMM, &CPU::ANC, 2 };
	// ARR
	m_jumpTable[0x6B] = { "*ARR", &CPU::addrIMM, &CPU::ARR, 2 };
	// AXS
	m_jumpTable[0x0B] = { "*AXS", &CPU::addrIMM, &CPU::AXS, 2 };
	// LAX
	m_jumpTable[0xA3] = { "*LAX", &CPU::addrINX, &CPU::LAX, 6 };
	m_jumpTable[0xA7] = { "*LAX", &CPU::addrZPI, &CPU::LAX, 3 };
	m_jumpTable[0xAF] = { "*LAX", &CPU::addrABS, &CPU::LAX, 4 };
	m_jumpTable[0xB3] = { "*LAX", &CPU::addrINY, &CPU::LAX, 5 };
	m_jumpTable[0xB7] = { "*LAX", &CPU::addrZPY, &CPU::LAX, 4 };
	m_jumpTable[0xBF] = { "*LAX", &CPU::addrABY, &CPU::LAX, 4 };
	// SAX
	m_jumpTable[0x83] = { "*SAX", &CPU::addrINX, &CPU::SAX, 6 };
	m_jumpTable[0x87] = { "*SAX", &CPU::addrZPI, &CPU::SAX, 3 };
	m_jumpTable[0x8F] = { "*SAX", &CPU::addrABS, &CPU::SAX, 4 };
	m_jumpTable[0x97] = { "*SAX", &CPU::addrZPY, &CPU::SAX, 4 };

	// duplicates
	m_jumpTable[0xEB] = { "*SBC", &CPU::addrIMM, &CPU::SBC, 2 };

	// MRW
	// DCP
	m_jumpTable[0xC3] = { "*DCP", &CPU::addrINX, &CPU::DCP, 8 };
	m_jumpTable[0xC7] = { "*DCP", &CPU::addrZPI, &CPU::DCP, 5 };
	m_jumpTable[0xCF] = { "*DCP", &CPU::addrABS, &CPU::DCP, 6 };
	m_jumpTable[0xD3] = { "*DCP", &CPU::addrINY, &CPU::DCP, 8 };
	m_jumpTable[0xD7] = { "*DCP", &CPU::addrZPX, &CPU::DCP, 6 };
	m_jumpTable[0xDB] = { "*DCP", &CPU::addrABY, &CPU::DCP, 7 };
	m_jumpTable[0xDF] = { "*DCP", &CPU::addrABX, &CPU::DCP, 7 };
	// ISC
	m_jumpTable[0xE3] = { "*ISB", &CPU::addrINX, &CPU::ISC, 8 };
	m_jumpTable[0xE7] = { "*ISB", &CPU::addrZPI, &CPU::ISC, 5 };
	m_jumpTable[0xEF] = { "*ISB", &CPU::addrABS, &CPU::ISC, 6 };
	m_jumpTable[0xF3] = { "*ISB", &CPU::addrINY, &CPU::ISC, 8 };
	m_jumpTable[0xF7] = { "*ISB", &CPU::addrZPX, &CPU::ISC, 6 };
	m_jumpTable[0xFB] = { "*ISB", &CPU::addrABY, &CPU::ISC, 7 };
	m_jumpTable[0xFF] = { "*ISB", &CPU::addrABX, &CPU::ISC, 7 };
	// SLO
	m_jumpTable[0x03] = { "*SLO", &CPU::addrINX, &CPU::SLO, 8 };
	m_jumpTable[0x07] = { "*SLO", &CPU::addrZPI, &CPU::SLO, 5 };
	m_jumpTable[0x0F] = { "*SLO", &CPU::addrABS, &CPU::SLO, 6 };
	m_jumpTable[0x13] = { "*SLO", &CPU::addrINY, &CPU::SLO, 8 };
	m_jumpTable[0x17] = { "*SLO", &CPU::addrZPX, &CPU::SLO, 6 };
	m_jumpTable[0x1B] = { "*SLO", &CPU::addrABY, &CPU::SLO, 7 };
	m_jumpTable[0x1F] = { "*SLO", &CPU::addrABX, &CPU::SLO, 7 };
	// RLA
	m_jumpTable[0x23] = { "*RLA", &CPU::addrINX, &CPU::RLA, 8 };
	m_jumpTable[0x27] = { "*RLA", &CPU::addrZPI, &CPU::RLA, 5 };
	m_jumpTable[0x2F] = { "*RLA", &CPU::addrABS, &CPU::RLA, 6 };
	m_jumpTable[0x33] = { "*RLA", &CPU::addrINY, &CPU::RLA, 8 };
	m_jumpTable[0x37] = { "*RLA", &CPU::addrZPX, &CPU::RLA, 6 };
	m_jumpTable[0x3B] = { "*RLA", &CPU::addrABY, &CPU::RLA, 7 };
	m_jumpTable[0x3F] = { "*RLA", &CPU::addrABX, &CPU::RLA, 7 };
	// SRE
	m_jumpTable[0x43] = { "*SRE", &CPU::addrINX, &CPU::SRE, 8 };
	m_jumpTable[0x47] = { "*SRE", &CPU::addrZPI, &CPU::SRE, 5 };
	m_jumpTable[0x4F] = { "*SRE", &CPU::addrABS, &CPU::SRE, 6 };
	m_jumpTable[0x53] = { "*SRE", &CPU::addrINY, &CPU::SRE, 8 };
	m_jumpTable[0x57] = { "*SRE", &CPU::addrZPX, &CPU::SRE, 6 };
	m_jumpTable[0x5B] = { "*SRE", &CPU::addrABY, &CPU::SRE, 7 };
	m_jumpTable[0x5F] = { "*SRE", &CPU::addrABX, &CPU::SRE, 7 };
	// RRA
	m_jumpTable[0x63] = { "*RRA", &CPU::addrINX, &CPU::RRA, 8 };
	m_jumpTable[0x67] = { "*RRA", &CPU::addrZPI, &CPU::RRA, 5 };
	m_jumpTable[0x6F] = { "*RRA", &CPU::addrABS, &CPU::RRA, 6 };
	m_jumpTable[0x73] = { "*RRA", &CPU::addrINY, &CPU::RRA, 8 };
	m_jumpTable[0x77] = { "*RRA", &CPU::addrZPX, &CPU::RRA, 6 };
	m_jumpTable[0x7B] = { "*RRA", &CPU::addrABY, &CPU::RRA, 7 };
	m_jumpTable[0x7F] = { "*RRA", &CPU::addrABX, &CPU::RRA, 7 };
}

void CPU::Step()
{
	// resetting temps
	m_oopsCycles = 0;
	m_canOops = false;
	m_done = false;
	m_discard = 0;

	if (m_cycle == 0)
	{
		m_opcode = readByte();

		m_currentInstr = m_jumpTable[m_opcode];

		m_cycle = m_currentInstr.cycles;
		// get address
		u16 addr = ( this->*m_currentInstr.addrMode )( );
		// execute instruction
		( this->*m_currentInstr.exec )( addr );

		if (m_canOops)
		{
			m_cycle += m_oopsCycles;
		}
		m_done = true;
	}
	m_totalCycles++;
	m_cycle--;
}

void CPU::Reset()
{
	m_A = 0;
	m_X = 0;
	m_Y = 0;
	m_S = 0xFD;
	m_P = 0x24;

	// 6502 reads memory at $FFFC/D
	u16 lo = memoryRead(RESET_VECTOR_LO);
	u16 hi = memoryRead(RESET_VECTOR_HI);

	m_PC = MAKE_WORD(hi, lo);
	//m_PC = 0xC000;

	// reset utility varibles
	m_canOops = false;
	m_oopsCycles = 0;
	m_discard = 0;
	// this takes 8 cycles
	m_cycle = 8;
}

void CPU::IRQ()
{
	if (checkFlag(ProcessorStatus::Flags::I))
		return;

	const u8 pcL = m_PC & 0x00ff;
	const u8 pcH = ( m_PC & 0xff00 ) >> 8;

	push(pcH);
	push(pcL);

	clearFlag(ProcessorStatus::Flags::B);

	push(m_P);

	setFlag(ProcessorStatus::Flags::I);
	const u8 irqL = memoryRead(IRQ_VECTOR_LO);
	const u8 irqH = memoryRead(IRQ_VECTOR_HI);

	m_PC = MAKE_WORD(irqH, irqL);
}

void CPU::NMI()
{
	const u8 pcL = m_PC & 0x00ff;
	const u8 pcH = ( m_PC & 0xff00 ) >> 8;

	push(pcH);
	push(pcL);

	clearFlag(ProcessorStatus::Flags::B);

	push(m_P);

	setFlag(ProcessorStatus::Flags::I);

	const u8 nmiL = memoryRead(NMI_VECTOR_LO);
	const u8 nmiH = memoryRead(NMI_VECTOR_HI);

	m_PC = MAKE_WORD(nmiH, nmiL);
}


u8 CPU::memoryRead(const u16 addr) const
{
#ifdef NES_EMU_DEBUG
	if (m_bus == nullptr)
		std::throw_with_nested(std::runtime_error("Cpu was not linked to a bus"));
#endif
	return m_bus->Read(addr);
}

void CPU::memoryWrite(const u16 addr, const u8 val) const
{
#ifdef NES_EMU_DEBUG
	if (m_bus == nullptr)
		std::throw_with_nested(std::runtime_error("Cpu was not linked to a bus"));
#endif
	m_bus->Write(addr, val);
}


u8 CPU::readByte()
{
	const u8 value = memoryRead(m_PC);
	m_PC++;
	return value;
}



u16 CPU::addrIMP()
{
	return 0;
}

u16 CPU::addrIMM()
{
	const u16 addr = m_PC++;

	return addr;
}

u16 CPU::addrZPI()
{
	u16 addr = readByte();
	addr &= 0x00FF;

	return addr;
}

u16 CPU::addrZPX()
{
	const u16 b = readByte();
	u16 addr = ( b + m_X );
	addr &= 0x00FF;

	return addr;
}

u16 CPU::addrZPY()
{
	const u16 b = readByte();
	u16 addr = ( b + m_Y );
	addr &= 0x00FF;

	return addr;
}


u16 CPU::addrABS()
{
	const u8 lo = readByte();
	const u8 hi = readByte();
	const u16 addr = MAKE_WORD(hi, lo);

	return addr;
}

u16 CPU::addrABX()
{
	const u16 lo = readByte();
	const u16 hi = readByte();

	u16 addr = MAKE_WORD(hi, lo);
	addr += m_X;

	if (( addr & 0xFF00 ) != ( hi << 8 ))
	{
		m_oopsCycles++;
	}

	return addr;
}

u16 CPU::addrABY()
{
	const u16 lo = readByte();
	const u16 hi = readByte();

	u16 addr = MAKE_WORD(hi, lo);
	addr += m_Y;

	if (( addr & 0xFF00 ) != ( hi << 8 ))
	{
		m_oopsCycles++;
	}

	return addr;
}

u16 CPU::addrIND()
{
	const u16 lo = readByte();
	const u16 hi = readByte();

	const u16 notyet = MAKE_WORD(hi, lo);

	u16 addr;

	// hardware bug
	if (lo == 0x00FF)
	{
		addr = MAKE_WORD(memoryRead(notyet & 0xFF00), memoryRead(notyet));
	}
	else
	{
		addr = MAKE_WORD(memoryRead(notyet + 1), memoryRead(notyet));
	}


	return addr;
}

u16 CPU::addrINX()
{
	const u16 b = readByte();
	const u16 base = ( b + m_X ) & 0x00FF;

	const u16 lo = memoryRead(( base & 0x00FF ));
	const u16 hi = memoryRead(( ( base + 1 ) & 0x00FF ));


	const u16 addr = MAKE_WORD(hi, lo);


	return addr;

}

u16 CPU::addrINY()
{
	const u16 base = readByte();

	u16 addr;
	if (base == 0xFF)
	{
		const u16 lo = memoryRead(0xFF);
		const u16 hi = memoryRead(0x00);
		addr = MAKE_WORD(hi, lo);
	}
	else
	{
		const u16 lo = memoryRead(base);
		const u16 hi = memoryRead(base + 1);
		addr = MAKE_WORD(hi, lo);
	}
	if (( ( addr + m_Y ) & 0xFF00 ) != ( addr & 0xFF00 ))
	{
		m_oopsCycles++;
	}
	const u16 addr_final = addr + m_Y;


	return addr_final;
}

u16 CPU::addrREL()
{
	u16 offset = readByte();

	// this contraption here is to emulate wrap around
	// could also be done by casting to i8, but I have grown attached
	// to the machine that killed Shinzo Abe as represented here
	// if bit 7 is set it will twos complement it to the negatives, but being u16

	if (offset & 0x80)
	{
		offset |= 0xff00;
	}


	const u16 addr = m_PC + offset;

	return addr;
}


u16 CPU::addrACC()
{

	return 0;
}

/*
 * Instruction Add with carry
 * m_A = m_A + M + C
 * flags: C, V, N, Z
 *
 * The overflow thing explained with a truth table
 * http://www.6502.org/tutorials/vflag.html#2.4
 *
 * the overflow will be set when a and m share the same sign but r is different
 * A^R will be true when a and r have different symbol
 * M^R will be true when m and r have different symbol
 * anding the two condition will get the result we want
 * then we extract the sign anding with 0x80
 * ┌-------┬---┬-----┬-----┬-----------┐
 * | A M R | V | A^R | M^R | A^R & A^M |
 * ├-------┼---┼-----┼-----┼-----------┤
 * | 0 0 0 | 0 |  0  |  0  |     0     |
 * | 0 0 1 | 1 |  1  |  1  |     1     |
 * | 0 1 0 | 0 |  0  |  1  |     0     |
 * | 0 1 1 | 0 |  1  |  0  |     0     |
 * | 1 0 0 | 0 |  1  |  0  |     0     |
 * | 1 0 1 | 0 |  0  |  1  |     0     |
 * | 1 1 0 | 1 |  1  |  1  |     1     |
 * | 1 1 1 | 0 |  0  |  0  |     0     |
 * └-------┴---┴-----┴-----┴-----------┘
 */
void CPU::ADC(const u16 addr)
{
	// hack to get overflow
	const u8 m = memoryRead(addr);

	ADD(m);

	m_canOops = true;
}

/*
 * Instruction AND
 * m_A = m_A & M
 * Flags: Z, N
 */
void CPU::AND(const u16 addr)
{
	const u8 m = memoryRead(addr);
	m_A &= m;
	setFlagIf(ProcessorStatus::Flags::Z, m_A == 0);
	setFlagIf(ProcessorStatus::Flags::N, m_A & 0x80);


	m_canOops = true;
}


/*
 * Instruction Arithemtic Shift Left
 * m_A = m_A * 2 | M = M * 2
 * Flags: Z, N, C
 */
void CPU::ASL(const u16 addr)
{
	u8 m;
	if (isImplied())
	{
		m = m_A;
	}
	else
	{
		m = memoryRead(addr);
	}

	setFlagIf(ProcessorStatus::Flags::C, m & 0x80);
	m <<= 1;
	m &= 0x00FF;
	setFlagIf(ProcessorStatus::Flags::Z, m == 0);
	setFlagIf(ProcessorStatus::Flags::N, m & 0x80);

	if (isImplied())
	{
		m_A = m;
	}
	else
	{
		memoryWrite(addr, m);
	}
}

/*
 * Instruction Branch if carry clear
 * if C == 0 --> PC = addr
 */
void CPU::BCC(const u16 addr)
{
	branchIfCond(addr, !checkFlag(ProcessorStatus::Flags::C));
}
/*
 * Instruction Branch if carry set
 * if C == 1 --> PC = addr
 */
void CPU::BCS(const u16 addr)
{
	branchIfCond(addr, checkFlag(ProcessorStatus::Flags::C));
}
/*
 * Instruction Branch if equal
 * if Z == 1 --> PC = addr
 */
void CPU::BEQ(const u16 addr)
{
	branchIfCond(addr, checkFlag(ProcessorStatus::Flags::Z));
}

/*
 * Instruction Bit Test
 * A = A & M
 * Flags: N = M7, V = M6
 */
void CPU::BIT(const u16 addr)
{
	const u8 m = memoryRead(addr);

	m_discard = m_A & m;

	setFlagIf(ProcessorStatus::Flags::Z, m_discard == 0);
	setFlagIf(ProcessorStatus::Flags::V, m & 0x40);
	setFlagIf(ProcessorStatus::Flags::N, m & 0x80);

}

/*
 * Instruction Branch if minus
 * if N == 1 --> PC = addr
 */
void CPU::BMI(const u16 addr)
{
	branchIfCond(addr, checkFlag(ProcessorStatus::Flags::N));
}

/*
 * Instruction Branch if not equals
 * if Z == 0 --> PC = addr
 */
void CPU::BNE(const u16 addr)
{
	branchIfCond(addr, !checkFlag(ProcessorStatus::Flags::Z));
}

/*
 * Instruction Branch if positive
 * if N == 0 --> PC = addr
 */
void CPU::BPL(const u16 addr)
{
	branchIfCond(addr, !checkFlag(ProcessorStatus::Flags::N));

}
/*
 * Instruction Force Interrupt
 * Pushes PC and P to the stack
 * Then loads the IRQ Interrupt vector ($FFFE/F)
 * into the PC and sets the B flag;
 *
 * This instruction is 7 cycles:
 * 1 read opcode
 * 2 read padding byte
 * 3 pushing PC hi byte
 * 4 pushing PC lo byte
 * 5 pushing P with B flag
 * 6 fetch low byte of irq vector from $FFFE
 * 7 fetch hi byte of irq vector from $FFFF
 */
void CPU::BRK(const u16 addr)
{
	// padding byte since BRK is a 2 byte instruction
	// even tho it uses implied mode
	m_PC++;
	// automatically setted when an interrupt is triggered
	const u8 pclo = m_PC & 0x00ff;
	const u8 pchi = ( m_PC & 0xff00 ) >> 8;

	push(pchi);
	push(pclo);

	push(m_P | ProcessorStatus::Flags::U | ProcessorStatus::Flags::B);
	setFlag(ProcessorStatus::Flags::I);

	const u8 lo = memoryRead(IRQ_VECTOR_LO);
	const u8 hi = memoryRead(IRQ_VECTOR_HI);

	m_PC = MAKE_WORD(hi, lo);
}

/*
 * Instruction Branch if overflow clear
 * if V == 0 --> PC = addr
 */
void CPU::BVC(const u16 addr)
{
	branchIfCond(addr, !checkFlag(ProcessorStatus::Flags::V));
}

/*
 * Instruction Branch if overflow set
 * if V == 1 --> PC = addr
 */
void CPU::BVS(const u16 addr)
{
	branchIfCond(addr, checkFlag(ProcessorStatus::Flags::V));
}
/*
 * Instruction Clear Carry Flags
 */
void CPU::CLC(const u16 addr)
{
	m_P &= ~ProcessorStatus::Flags::C;
}

/*
 * Instruction Clear Decimal Mode
 */
void CPU::CLD(const u16 addr)
{
	m_P &= ~ProcessorStatus::Flags::D;
}

/*
 * Instruction Clear Interrupt Disable
 */
void CPU::CLI(const u16 addr)
{
	m_P &= ~ProcessorStatus::Flags::I;
}

/*
 * Instruction Clear Overflow Flags
 */
void CPU::CLV(const u16 addr)
{
	m_P &= ~ProcessorStatus::Flags::V;
}

/*
 * Instruction Compare
 * A - M
 * result is discarded
 * flags: Z, C, N
 */
void CPU::CMP(const u16 addr)
{
	const u8 m = memoryRead(addr);

	m_discard = m_A - m;

	setFlagIf(ProcessorStatus::Flags::C, m_A >= m);
	setFlagIf(ProcessorStatus::Flags::Z, ( m_discard & 0x00FF ) == 0x000);
	setFlagIf(ProcessorStatus::Flags::N, m_discard & 0x80);


	m_canOops = true;
}

/*
 * Instruction Compare X Register
 * X - M
 * result is discarded
 * flags: Z, C, N
 */
void CPU::CPX(const u16 addr)
{
	const u8 m = memoryRead(addr);

	m_discard = m_X - m;

	setFlagIf(ProcessorStatus::Flags::C, m_X >= m);
	setFlagIf(ProcessorStatus::Flags::Z, m_X == m);
	setFlagIf(ProcessorStatus::Flags::N, m_discard & 0x80);

}

/*
 * Instruction Compare Y Register
 * Y - M
 * result is discarded
 * flags: Z, C, N
 */
void CPU::CPY(const u16 addr)
{
	const u8 m = memoryRead(addr);

	m_discard = m_Y - m;

	setFlagIf(ProcessorStatus::Flags::C, m_Y >= m);
	setFlagIf(ProcessorStatus::Flags::Z, m_Y == m);
	setFlagIf(ProcessorStatus::Flags::N, m_discard & 0x80);
}

/*
 * Instruction Decrement Memory
 * M = M - 1
 * flags: N, Z
 */
void CPU::DEC(const u16 addr)
{
	u8 m = memoryRead(addr);

	m = ( m - 1 ) & 0x00ff;

	setFlagIf(ProcessorStatus::Flags::N, m & 0x80);
	setFlagIf(ProcessorStatus::Flags::Z, m == 0);

	memoryWrite(addr, m);
}

/*
 * Instruction Decrement X Register
 * X = X - 1
 * flags: N, Z
 */
void CPU::DEX(const u16 addr)
{
	m_X = ( m_X - 1 ) & 0x00ff;

	setFlagIf(ProcessorStatus::Flags::N, m_X & 0x80);
	setFlagIf(ProcessorStatus::Flags::Z, m_X == 0);
}

/*
 * Instruction Decrement Y Register
 * Y = Y - 1
 * flags: N, Z
 */
void CPU::DEY(const u16 addr)
{
	m_Y = ( m_Y - 1 ) & 0x00ff;

	setFlagIf(ProcessorStatus::Flags::N, m_Y & 0x80);
	setFlagIf(ProcessorStatus::Flags::Z, m_Y == 0);
}

/*
 * Instruction Exclusive or
 * m_A = m_A ^ M
 * Flags: Z, N
 */
void CPU::EOR(const u16 addr)
{
	const u8 m = memoryRead(addr);

	m_A ^= m;

	setFlagIf(ProcessorStatus::Flags::Z, m_A == 0);
	setFlagIf(ProcessorStatus::Flags::N, m_A & 0x80);

	m_canOops = true;
}

/*
 * Instruction Increment Memory
 * M = M + 1 (wrpas if m = 0xff)
 * flags: Z, N
 */
void CPU::INC(const u16 addr)
{
	u8 m = memoryRead(addr);

	m = ( m + 1 ) & 0x00ff;

	setFlagIf(ProcessorStatus::Flags::N, m & 0x80);
	setFlagIf(ProcessorStatus::Flags::Z, m == 0);

	memoryWrite(addr, m);
}

/*
 * Instruction Increment X Register
 * X = X + 1 (wrpas if X = 0xff)
 * flags: Z, N
 */
void CPU::INX(const u16 addr)
{
	m_X = ( m_X + 1 ) & 0x00ff;

	setFlagIf(ProcessorStatus::Flags::N, m_X & 0x80);
	setFlagIf(ProcessorStatus::Flags::Z, m_X == 0);
}

/*
 * Instruction Increment Y Register
 * Y = Y + 1 (wraps if Y = 0xff)
 * flags: Z, N
 */
void CPU::INY(const u16 addr)
{
	m_Y = ( m_Y + 1 ) & 0x00ff;

	setFlagIf(ProcessorStatus::Flags::N, m_Y & 0x80);
	setFlagIf(ProcessorStatus::Flags::Z, m_Y == 0);
}

/*
 * Instruction Jump
 * PC = M
 */
void CPU::JMP(const u16 addr)
{
	m_PC = addr;
}

/*
 * Instruction Jump from Subroutine
 * Pushes pc - 1 to the stack
 * and sets the pc to M
 */
void CPU::JSR(const u16 addr)
{
	const u16 temp = m_PC - 1;

	push_word(temp);

	m_PC = addr;
}

/*
 * Instruction Load Accumulator
 * A = M
 * flags: Z, N
 */
void CPU::LDA(const u16 addr)
{
	const u8 m = memoryRead(addr);
	set_register(m_A, m);

	m_canOops = true;
}

/*
 * Instruction Load X
 * X = M
 * flags: Z, N
 */
void CPU::LDX(const u16 addr)
{
	const u8 m = memoryRead(addr);

	transferRegTo(m, m_X);

	m_canOops = true;
}

/*
 * Instruction Load Y
 * X = M
 * flags: Z, N
 */
void CPU::LDY(const u16 addr)
{
	const u8 m = memoryRead(addr);

	transferRegTo(m, m_Y);

	m_canOops = true;
}

/*
 * Instruction Logical Shift Right
 * A = A/2 | M = M/2
 * flags: C, Z, N
 */
void CPU::LSR(const u16 addr)
{
	u8 m;
	if (isImplied())
	{
		m = m_A;
	}
	else
	{
		m = memoryRead(addr);
	}


	setFlagIf(ProcessorStatus::Flags::C, m & 1);
	m >>= 1;
	m &= 0x00FF;
	setFlagIf(ProcessorStatus::Flags::Z, m == 0);
	setFlagIf(ProcessorStatus::Flags::N, m & 0x80);

	if (isImplied())
	{
		m_A = m;
	}
	else
	{
		memoryWrite(addr, m);
	}
}

/*
 * Instruction No Operation
 * Does nothing
 */
void CPU::NOP(const u16 addr)
{
}

/*
 * Instruction Inclusive or
 * m_A = m_A | M
 * Flags: Z, N
 */
void CPU::ORA(const u16 addr)
{
	const u8 m = memoryRead(addr);

	m_A |= m;

	setFlagIf(ProcessorStatus::Flags::Z, m_A == 0);
	setFlagIf(ProcessorStatus::Flags::N, m_A & 0x80);

	m_canOops = true;
}

/*
 * Instruction Push Accumulator
 * Pushes accumulator to the stack
 *
 * The stack is hardcoded in page $01 ($0100-$01FF)
 *
 * Important, The 6502 uses a reverse stack
 */
void CPU::PHA(const u16 addr)
{
	push(m_A);
}

/*
 * Instruction Push Processor status
 * Pushes accumulator to the stack
 *
 * also pushes B flag
 */
void CPU::PHP(const u16 addr)
{
	push(m_P | ProcessorStatus::Flags::B | ProcessorStatus::Flags::U);
}

/*
 * Instruction Pull Accumulator
 * Pulls accumulator from the stack
 * Uses flag N, Z for some reason
 */
void CPU::PLA(const u16 addr)
{
	set_register(m_A, pop());
}
/*
 * Instruction Pull Processor Status
 * Pulls Processor status from the stack
 */
void CPU::PLP(const u16 addr)
{
	set_p(pop());
	setFlag(ProcessorStatus::Flags::U);
}


/*
 * Instruction Rotate Left
 * move each byte A|M one place to the left
 * bit 0 is filled with carry
 * carry is filled with bit 7
 * flags: C, Z, N
 */
void CPU::ROL(const u16 addr)
{
	u16 m;
	if (isImplied())
	{
		m = m_A;
	}
	else
	{
		m = memoryRead(addr);
	}
	// we are in 16 bit realm so if we have
	// 0000 0000 1011 10001
	// and we rotate left we should end with
	// 0000 0001 0111 00010
	// anything above bit 7 will be cut in 8 bits
	// but in 16 we can store it to check flags
	m <<= 1;
	if (checkFlag(ProcessorStatus::Flags::C))
	{
		m |= 1;
	}

	setFlagIf(ProcessorStatus::Flags::C, m > 0xff);
	m &= 0x00FF;
	setFlagIf(ProcessorStatus::Flags::N, m & 0x80);
	setFlagIf(ProcessorStatus::Flags::Z, m == 0);

	if (isImplied())
	{
		m_A = static_cast<u8>( m );
	}
	else
	{
		memoryWrite(addr, static_cast<u8>( m ));
	}
}

/*
 * Instruction Rotate Right
 * move each byte A|M one place to the right
 * bit 7 is filled with carry
 * carry is filled with bit 0
 * flags: C, Z, N
 */
void CPU::ROR(const u16 addr)
{
	u16 m;
	if (isImplied())
	{
		m = m_A;
	}
	else
	{
		m = memoryRead(addr);
	}

	// we set bit 8 in case of carry so we can 
	// store it after shifting
	if (checkFlag(ProcessorStatus::Flags::C))
	{
		m |= 0x100;
	}
	setFlagIf(ProcessorStatus::Flags::C, m & 1);
	m >>= 1;

	m &= 0x00FF;
	setFlagIf(ProcessorStatus::Flags::N, m & 0x80);
	setFlagIf(ProcessorStatus::Flags::Z, m == 0);

	if (isImplied())
	{
		m_A = static_cast<u8>( m );
	}
	else
	{
		memoryWrite(addr, static_cast<u8>( m ));
	}
}
/*
 * Instruction Return from Interrupt
 * Used at the end of an interrupt processing routine
 * Works oppostie of BRK, pulls P and PC from the stack
 *
 * Also discard B flag
 */
void CPU::RTI(u16 addr)
{
	set_p(pop());
	setFlag(ProcessorStatus::Flags::U);
	m_PC = pop_word();
}

/*
 * Instruction Return from Subroutine
 * pulls the PC minus one from the stack
 */
void CPU::RTS(u16 addr)
{

	u16 m = pop_word();

	m_PC = m + 1;
}

/*
 * Instruction Substract with Carry
 * Substracts the not of the carry flag for some reason
 * A = A - M - (1 - C)
 * flags: Z, C, V, N
 *
 * The overflow thing explained with a truth table
 * http://www.6502.org/tutorials/vflag.html#2.4
 *
 * the overflow will be set when a and m share the same sign but r is different
 * A^R will be true when a and r have different symbol
 * M^R will be true when m and r have different symbol
 * anding the two condition will get the result we want
 * then we extract the sign anding with 0x80
 * ┌-------┬---┬-----┬-----┬-----------┐
 * | A M R | V | A^R | M^R | A^R & M^R |
 * ├-------┼---┼-----┼-----┼-----------┤
 * | 0 0 0 | 0 |  0  |  0  |     0     |
 * | 0 0 1 | 1 |  1  |  1  |     1     |
 * | 0 1 0 | 0 |  0  |  1  |     0     |
 * | 0 1 1 | 0 |  1  |  0  |     0     |
 * | 1 0 0 | 0 |  1  |  0  |     0     |
 * | 1 0 1 | 0 |  0  |  1  |     0     |
 * | 1 1 0 | 1 |  1  |  1  |     1     |
 * | 1 1 1 | 0 |  0  |  0  |     0     |
 * └-------┴---┴-----┴-----┴-----------┘
 */
void CPU::SBC(const u16 addr)
{
	const u8 m = memoryRead(addr) ^ 0xFF;
	ADD(m);

	m_canOops = true;
}

/*
 * Instruction Set Carry Flags
 */
void CPU::SEC(u16 addr)
{
	m_P |= ProcessorStatus::Flags::C;
}

/*
 * Instruction Set Decimal Mode
 */
void CPU::SED(u16 addr)
{
	m_P |= ProcessorStatus::Flags::D;
}

/*
 * Instruction Set Insterrupt Disable
 */
void CPU::SEI(u16 addr)
{
	m_P |= ProcessorStatus::Flags::I;
}

/*
 * Instruction Store Accumulator
 * M = A
 */
void CPU::STA(const u16 addr)
{
	memoryWrite(addr, m_A);
}

/*
 * Instruction Store Index register X
 * M = X
 */
void CPU::STX(const u16 addr)
{
	memoryWrite(addr, m_X);
}

/*
 * Instruction Store Index register Y
 * M = Y
 */
void CPU::STY(const u16 addr)
{
	memoryWrite(addr, m_Y);
}

/*
 * Instruction Transfer Accumulator to X
 * X = A
 * flags: Z, N
 */
void CPU::TAX(const u16 addr)
{
	set_register(m_X, m_A);
}

/*
 * Instruction Transfer Accumulator to Y
 * Y = A
 * flags: Z, N
 */
void CPU::TAY(const u16 addr)
{
	transferRegTo(m_A, m_Y);
}

/*
 * Instruction Transfer Stack pointer to X
 * X = S
 * flags: Z, N
 */
void CPU::TSX(const u16 addr)
{
	set_register(m_X, m_S);
}

/*
 * Instruction Transfer X to Accumulator
 * A = X
 * flags: Z, N
 */
void CPU::TXA(const u16 addr)
{
	set_register(m_A, m_X);
}

/*
 * Instruction Transfer X to Stack pointer
 * S = X
 */
void CPU::TXS(const u16 addr)
{
	m_S = m_X;
}

/*
 * Instruction Transfer Y to Accumulator
 * A = Y
 * flags: Z, N
 */
void CPU::TYA(const u16 addr)
{
	set_register(m_A, m_Y);
}

// Illegal opcode Skip Byte
void CPU::SKB(const u16 addr)
{
	// reads next byte and discards it
	m_discard = memoryRead(addr);
}

// Illegal opcode Skip Word
void CPU::IGN(const u16 addr)
{
	// reads next word and discards it
	m_discard = memoryRead(addr);

	m_canOops = true;
}

void CPU::ALR(const u16 addr)
{
	AND(addr);
	setFlagIf(ProcessorStatus::Flags::C, m_A & 0x01);
	set_register(m_A, m_A >> 1);

}

void CPU::ANC(const u16 addr)
{
	AND(addr);
	setFlagIf(ProcessorStatus::Flags::C, addr & 0x80);
}

void CPU::ARR(const u16 addr)
{
	AND(addr);
	// we set bit 8 in case of carry so we can 
	// store it after shifting
	set_register(m_A, ( m_A >> 1 ) | ( checkFlag(ProcessorStatus::Flags::C) << 7 ));


}

void CPU::AXS(const u16 addr)
{
	m_discard = m_A & m_X;
	set_register(m_X, m_discard - addr);
	setFlagIf(ProcessorStatus::Flags::C, m_discard >= m_X);
}

void CPU::LAX(const u16 addr)
{
	LDA(addr);
	TAX(addr);
}

void CPU::SAX(const u16 addr)
{
	m_discard = m_A & m_X;

	memoryWrite(addr, m_discard);
}

void CPU::DCP(const u16 addr)
{
	u8 m = memoryRead(addr);
	//memoryWrite(addr, m); // dummy read

	m--;
	m_discard = m_A - m;

	setFlagIf(ProcessorStatus::Flags::C, m_A >= m);
	setFlagIf(ProcessorStatus::Flags::Z, ( m_discard & 0x00FF ) == 0x000);
	setFlagIf(ProcessorStatus::Flags::N, m_discard & 0x80);

	memoryWrite(addr, m);
}

// also known as isb
void CPU::ISC(const u16 addr)
{
	u8 m = memoryRead(addr);

	//memoryWrite(addr, m); // dumy write
	m++;
	ADD(m ^ 0xFF);
	memoryWrite(addr, m);
}

void CPU::RLA(const u16 addr)
{
	u8 m = memoryRead(addr);


	//memoryWrite(addr, m); // dumy write
	bool carry = checkFlag(ProcessorStatus::Flags::C);
	setFlagIf(ProcessorStatus::Flags::C, m & 0x80);
	u8 shifted = ( m << 1 ) | ( carry ? 1 : 0 );
	set_register(m_A, m_A & shifted);
	memoryWrite(addr, shifted);
}

void CPU::RRA(const u16 addr)
{
	u8 m = memoryRead(addr);

	//memoryWrite(addr, m); // dumy write
	bool carry = checkFlag(ProcessorStatus::Flags::C);
	setFlagIf(ProcessorStatus::Flags::C, m & 0x01);
	u8 shifted = ( m >> 1 ) | ( carry << 7 );
	ADD(shifted);
	memoryWrite(addr, shifted);
}

void CPU::SLO(const u16 addr)
{
	u8 m = memoryRead(addr);

	//memoryWrite(addr, m); // dummy write
	setFlagIf(ProcessorStatus::Flags::C, m & 0x80);

	u8 shifted = m << 1;
	set_register(m_A, m_A | shifted);
	memoryWrite(addr, shifted);
}

void CPU::SRE(const u16 addr)
{
	// ROL & AND
	u8 m = memoryRead(addr);

	//memoryWrite(addr, m); // dummy write
	setFlagIf(ProcessorStatus::Flags::C, m & 0x01);

	u8 shifted = m >> 1;
	set_register(m_A, m_A ^ shifted);
	memoryWrite(addr, shifted);
}

void CPU::ADD(u8 val)
{
	const u16 a = m_A;

	u16 r = a + val + static_cast<u16>( checkFlag(ProcessorStatus::Flags::C) );

	setFlagIf(ProcessorStatus::Flags::N, r & 0x80);
	setFlagIf(ProcessorStatus::Flags::Z, ( r & 0x00ff ) == 0);
	setFlagIf(ProcessorStatus::Flags::V, ( a ^ r ) & ( val ^ r ) & 0x0080);
	setFlagIf(ProcessorStatus::Flags::C, r & 0xFF00);

	m_A = r & 0x00ff;
}

void CPU::STP(const u16 addr)
{
	// commit sudoku
	// 
	//std::throw_with_nested(std::runtime_error("Illegal instruction"));
	std::println("Called STP opcode [{:#02x}] at PC: [{:d}]", m_opcode, m_PC);

	throw std::runtime_error(std::format("Called STP opcode [{:#02x}] at PC: [{:d}]", m_opcode, m_PC));
}

void CPU::setFlagIf(const u8 flag, const bool cond)
{
	if (cond)
	{
		setFlag(flag);
	}
	else
	{
		clearFlag(flag);
	}
}

void CPU::setFlag(const u8 flag)
{
	m_P |= flag;
}

void CPU::clearFlag(const u8 flag)
{
	m_P &= ~flag;
}

bool CPU::checkFlag(const u8 flag) const
{
	return ( m_P & flag ) == flag;
}

bool CPU::isImplied() const
{
	return m_currentInstr.addrMode == &CPU::addrIMP || m_currentInstr.addrMode == &CPU::addrACC;
}

void CPU::branchIfCond(const u16 addr, bool cond)
{
	// flag is clear
	if (cond)
	{
		m_cycle++;

		const u8 page = static_cast<u8>( m_PC & 0xff00 );

		m_PC = addr;
		// extra cycle for page change
		if (page != ( m_PC & 0xff00 ))
		{
			m_cycle++;
		}
	}
}

void CPU::transferRegTo(const u8 from, u8& to)
{
	to = from;
	setFlagIf(ProcessorStatus::Flags::Z, to == 0);
	setFlagIf(ProcessorStatus::Flags::N, to & 0x80);
}

void CPU::push(const u8 val)
{
	memoryWrite(STACK_VECTOR + m_S, val);
	m_S = m_S - 1;
}

void CPU::push_word(u16 val)
{
	push(static_cast<u8>( val >> 8 ));
	push(static_cast<u8>( val ));
}

u8 CPU::pop()
{
	m_S = m_S + 1;
	return memoryRead(STACK_VECTOR + m_S);
}

u16 CPU::pop_word()
{
	u8 lo = pop();
	u8 hi = pop();
	return lo | hi << 8;
}

void CPU::set_register(u8& reg, const u8 val)
{
	setFlagIf(ProcessorStatus::Flags::Z, val == 0);
	setFlagIf(ProcessorStatus::Flags::N, val & 0x80);
	reg = val;
}

void CPU::set_p(const u8 val)
{
	// SET TO CF
	m_P = val & 0xCF;
}

}