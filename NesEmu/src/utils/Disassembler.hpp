#ifndef A6502_DISSASEMBLER_HEADER
#define A6502_DISSASEMBLER_HEADER

#include "internals/Core.hpp"
#include <string>
#include <vector>

namespace A6502
{

enum class InstructionName
{
	// Legal
	ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, CLC,
	CLD, CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, EOR, INC, INX, INY, JMP,
	JSR, LDA, LDX, LDY, LSR, NOP, ORA, PHA, PHP, PLA, PLP, ROL, ROR, RTI,
	RTS, SBC, SEC, SED, SEI, STA, STX, STY, TAX, TAY, TSX, TXA, TXS, TYA,
	// Illegal
	ALR, ANC, ARR, AXS, LAX, SAX,
	DCP, ISC, RLA, RRA, SLO, SRE,
	DOP, TOP,
	STP,
	// don't know what are these
	XAA, AHX, TAS, SHY, SHX, LAS,
};

enum class AddressingModeName
{
	IMP,
	ACC,
	IMM,
	ZPI,
	ZPX,
	ZPY,
	REL,
	ABS,
	ABX,
	ABY,
	IND,
	INX,
	INY,
};

struct Opcode
{
	InstructionName instruction;
	AddressingModeName mode;
};

struct Disassembly
{
	std::string repr;
	std::string explanation_instr;
	std::string explanation_mode;
	u8 bytes;
};

std::string FormatAddressingMode(AddressingModeName am, u16 val=0);
std::string GetInstructionName(InstructionName i);

std::string GetAddressingModeExplanation(AddressingModeName am);
std::string GetInstructionExplanation(InstructionName i);

u8 GetBytesForAddressingMode(AddressingModeName am);

class Disassembler
{
public:
	Disassembler& ConnectBus(Emu::Bus* bus);

	std::vector<Disassembly> Disassemble();

private:
	Emu::Bus* m_bus{ nullptr };

	u32 m_pc;

	static inline std::array<Opcode, 0x100> s_instructions{ {
		{InstructionName::BRK, AddressingModeName::IMP},/*$00*/
		{InstructionName::ORA, AddressingModeName::INX},/*$01*/
		{InstructionName::STP, AddressingModeName::IMP},/*$02*/
		{InstructionName::SLO, AddressingModeName::INX},/*$03*/
		{InstructionName::DOP, AddressingModeName::ZPI},/*$04*/
		{InstructionName::ORA, AddressingModeName::ZPI},/*$05*/
		{InstructionName::ASL, AddressingModeName::ZPI},/*$06*/
		{InstructionName::SLO, AddressingModeName::ZPI},/*$07*/
		{InstructionName::PHP, AddressingModeName::IMP},/*$08*/
		{InstructionName::ORA, AddressingModeName::IMM},/*$09*/
		{InstructionName::ASL, AddressingModeName::IMP},/*$0A*/
		{InstructionName::ANC, AddressingModeName::IMM},/*$0B*/
		{InstructionName::TOP, AddressingModeName::ABS},/*$0C*/
		{InstructionName::ORA, AddressingModeName::ABS},/*$0D*/
		{InstructionName::ASL, AddressingModeName::ABS},/*$0E*/
		{InstructionName::SLO, AddressingModeName::ABS},/*$0F*/
		{InstructionName::BPL, AddressingModeName::REL},/*$10*/
		{InstructionName::ORA, AddressingModeName::INY},/*$11*/
		{InstructionName::STP, AddressingModeName::IMP},/*$12*/
		{InstructionName::SLO, AddressingModeName::INY},/*$13*/
		{InstructionName::DOP, AddressingModeName::ZPX},/*$14*/
		{InstructionName::ORA, AddressingModeName::ZPX},/*$15*/
		{InstructionName::ASL, AddressingModeName::ZPX},/*$16*/
		{InstructionName::SLO, AddressingModeName::ZPX},/*$17*/
		{InstructionName::CLC, AddressingModeName::IMP},/*$18*/
		{InstructionName::ORA, AddressingModeName::ABY},/*$19*/
		{InstructionName::NOP, AddressingModeName::IMP},/*$1A*/
		{InstructionName::SLO, AddressingModeName::ABY},/*$1B*/
		{InstructionName::TOP, AddressingModeName::ABX},/*$1C*/
		{InstructionName::ORA, AddressingModeName::ABX},/*$1D*/
		{InstructionName::ASL, AddressingModeName::ABX},/*$1E*/
		{InstructionName::SLO, AddressingModeName::ABX},/*$1F*/
		{InstructionName::JSR, AddressingModeName::ABS},/*$20*/
		{InstructionName::AND, AddressingModeName::INX},/*$21*/
		{InstructionName::STP, AddressingModeName::IMP},/*$22*/
		{InstructionName::RLA, AddressingModeName::INX},/*$23*/
		{InstructionName::BIT, AddressingModeName::ZPI},/*$24*/
		{InstructionName::AND, AddressingModeName::ZPI},/*$25*/
		{InstructionName::ROL, AddressingModeName::ZPI},/*$26*/
		{InstructionName::RLA, AddressingModeName::ZPI},/*$27*/
		{InstructionName::PLP, AddressingModeName::IMP},/*$28*/
		{InstructionName::AND, AddressingModeName::IMM},/*$29*/
		{InstructionName::ROL, AddressingModeName::IMP},/*$2A*/
		{InstructionName::ANC, AddressingModeName::IMM},/*$2B*/
		{InstructionName::BIT, AddressingModeName::ABS},/*$2C*/
		{InstructionName::AND, AddressingModeName::ABS},/*$2D*/
		{InstructionName::ROL, AddressingModeName::ABS},/*$2E*/
		{InstructionName::RLA, AddressingModeName::ABS},/*$2F*/
		{InstructionName::BMI, AddressingModeName::REL},/*$30*/
		{InstructionName::AND, AddressingModeName::INY},/*$31*/
		{InstructionName::STP, AddressingModeName::IMP},/*$32*/
		{InstructionName::RLA, AddressingModeName::INY},/*$33*/
		{InstructionName::DOP, AddressingModeName::ZPX},/*$34*/
		{InstructionName::AND, AddressingModeName::ZPX},/*$35*/
		{InstructionName::ROL, AddressingModeName::ZPX},/*$36*/
		{InstructionName::RLA, AddressingModeName::ZPX},/*$37*/
		{InstructionName::SEC, AddressingModeName::IMP},/*$38*/
		{InstructionName::AND, AddressingModeName::ABY},/*$39*/
		{InstructionName::NOP, AddressingModeName::IMP},/*$3A*/
		{InstructionName::RLA, AddressingModeName::ABY},/*$3B*/
		{InstructionName::TOP, AddressingModeName::ABX},/*$3C*/
		{InstructionName::AND, AddressingModeName::ABX},/*$3D*/
		{InstructionName::ROL, AddressingModeName::ABX},/*$3E*/
		{InstructionName::RLA, AddressingModeName::ABX},/*$3F*/
		{InstructionName::RTI, AddressingModeName::IMP},/*$40*/
		{InstructionName::EOR, AddressingModeName::INX},/*$41*/
		{InstructionName::STP, AddressingModeName::IMP},/*$42*/
		{InstructionName::SRE, AddressingModeName::INX},/*$43*/
		{InstructionName::DOP, AddressingModeName::ZPI},/*$44*/
		{InstructionName::EOR, AddressingModeName::ZPI},/*$45*/
		{InstructionName::LSR, AddressingModeName::ZPI},/*$46*/
		{InstructionName::SRE, AddressingModeName::ZPI},/*$47*/
		{InstructionName::PHA, AddressingModeName::IMP},/*$48*/
		{InstructionName::EOR, AddressingModeName::IMM},/*$49*/
		{InstructionName::LSR, AddressingModeName::IMP},/*$4A*/
		{InstructionName::ALR, AddressingModeName::IMM},/*$4B*/
		{InstructionName::JMP, AddressingModeName::ABS},/*$4C*/
		{InstructionName::EOR, AddressingModeName::ABS},/*$4D*/
		{InstructionName::LSR, AddressingModeName::ABS},/*$4E*/
		{InstructionName::SRE, AddressingModeName::ABS},/*$4F*/
		{InstructionName::BVC, AddressingModeName::REL},/*$50*/
		{InstructionName::EOR, AddressingModeName::INY},/*$51*/
		{InstructionName::STP, AddressingModeName::IMP},/*$52*/
		{InstructionName::SRE, AddressingModeName::INY},/*$53*/
		{InstructionName::DOP, AddressingModeName::ZPX},/*$54*/
		{InstructionName::EOR, AddressingModeName::ZPX},/*$55*/
		{InstructionName::LSR, AddressingModeName::ZPX},/*$56*/
		{InstructionName::SRE, AddressingModeName::ZPX},/*$57*/
		{InstructionName::CLI, AddressingModeName::IMP},/*$58*/
		{InstructionName::EOR, AddressingModeName::ABY},/*$59*/
		{InstructionName::NOP, AddressingModeName::IMP},/*$5A*/
		{InstructionName::SRE, AddressingModeName::ABY},/*$5B*/
		{InstructionName::TOP, AddressingModeName::ABX},/*$5C*/
		{InstructionName::EOR, AddressingModeName::ABX},/*$5D*/
		{InstructionName::LSR, AddressingModeName::ABX},/*$5E*/
		{InstructionName::SRE, AddressingModeName::ABX},/*$5F*/
		{InstructionName::RTS, AddressingModeName::IMP},/*$60*/
		{InstructionName::ADC, AddressingModeName::INX},/*$61*/
		{InstructionName::STP, AddressingModeName::IMP},/*$62*/
		{InstructionName::RRA, AddressingModeName::INX},/*$63*/
		{InstructionName::DOP, AddressingModeName::ZPI},/*$64*/
		{InstructionName::ADC, AddressingModeName::ZPI},/*$65*/
		{InstructionName::ROR, AddressingModeName::ZPI},/*$66*/
		{InstructionName::RRA, AddressingModeName::ZPI},/*$67*/
		{InstructionName::PLA, AddressingModeName::IMP},/*$68*/
		{InstructionName::ADC, AddressingModeName::IMM},/*$69*/
		{InstructionName::ROR, AddressingModeName::IMP},/*$6A*/
		{InstructionName::ARR, AddressingModeName::IMM},/*$6B*/
		{InstructionName::JMP, AddressingModeName::IND},/*$6C*/
		{InstructionName::ADC, AddressingModeName::ABS},/*$6D*/
		{InstructionName::ROR, AddressingModeName::ABS},/*$6E*/
		{InstructionName::RRA, AddressingModeName::ABS},/*$6F*/
		{InstructionName::BVS, AddressingModeName::REL},/*$70*/
		{InstructionName::ADC, AddressingModeName::INY},/*$71*/
		{InstructionName::STP, AddressingModeName::IMP},/*$72*/
		{InstructionName::RRA, AddressingModeName::INY},/*$73*/
		{InstructionName::DOP, AddressingModeName::ZPX},/*$74*/
		{InstructionName::ADC, AddressingModeName::ZPX},/*$75*/
		{InstructionName::ROR, AddressingModeName::ZPX},/*$76*/
		{InstructionName::RRA, AddressingModeName::ZPX},/*$77*/
		{InstructionName::SEI, AddressingModeName::IMP},/*$78*/
		{InstructionName::ADC, AddressingModeName::ABY},/*$79*/
		{InstructionName::NOP, AddressingModeName::IMP},/*$7A*/
		{InstructionName::RRA, AddressingModeName::ABY},/*$7B*/
		{InstructionName::TOP, AddressingModeName::ABX},/*$7C*/
		{InstructionName::ADC, AddressingModeName::ABX},/*$7D*/
		{InstructionName::ROR, AddressingModeName::ABX},/*$7E*/
		{InstructionName::RRA, AddressingModeName::ABX},/*$7F*/
		{InstructionName::DOP, AddressingModeName::IMM},/*$80*/
		{InstructionName::STA, AddressingModeName::INX},/*$81*/
		{InstructionName::DOP, AddressingModeName::IMM},/*$82*/
		{InstructionName::SAX, AddressingModeName::INX},/*$83*/
		{InstructionName::STY, AddressingModeName::ZPI},/*$84*/
		{InstructionName::STA, AddressingModeName::ZPI},/*$85*/
		{InstructionName::STX, AddressingModeName::ZPI},/*$86*/
		{InstructionName::SAX, AddressingModeName::ZPI},/*$87*/
		{InstructionName::DEY, AddressingModeName::IMP},/*$88*/
		{InstructionName::DOP, AddressingModeName::IMM},/*$89*/
		{InstructionName::TXA, AddressingModeName::IMP},/*$8A*/
		{InstructionName::XAA, AddressingModeName::IMM},/*$8B*/
		{InstructionName::STY, AddressingModeName::ABS},/*$8C*/
		{InstructionName::STA, AddressingModeName::ABS},/*$8D*/
		{InstructionName::STX, AddressingModeName::ABS},/*$8E*/
		{InstructionName::SAX, AddressingModeName::ABS},/*$8F*/
		{InstructionName::BCC, AddressingModeName::REL},/*$90*/
		{InstructionName::STA, AddressingModeName::INY},/*$91*/
		{InstructionName::STP, AddressingModeName::IMP},/*$92*/
		{InstructionName::AHX, AddressingModeName::INY},/*$93*/
		{InstructionName::STY, AddressingModeName::ZPX},/*$94*/
		{InstructionName::STA, AddressingModeName::ZPX},/*$95*/
		{InstructionName::STX, AddressingModeName::ZPY},/*$96*/
		{InstructionName::SAX, AddressingModeName::ZPY},/*$97*/
		{InstructionName::TYA, AddressingModeName::IMP},/*$98*/
		{InstructionName::STA, AddressingModeName::ABY},/*$99*/
		{InstructionName::TXS, AddressingModeName::IMP},/*$9A*/
		{InstructionName::TAS, AddressingModeName::ABY},/*$9B*/
		{InstructionName::SHY, AddressingModeName::ABX},/*$9C*/
		{InstructionName::STA, AddressingModeName::ABX},/*$9D*/
		{InstructionName::SHX, AddressingModeName::ABY},/*$9E*/
		{InstructionName::AHX, AddressingModeName::ABY},/*$9F*/
		{InstructionName::LDY, AddressingModeName::IMM},/*$A0*/
		{InstructionName::LDA, AddressingModeName::INX},/*$A1*/
		{InstructionName::LDX, AddressingModeName::IMM},/*$A2*/
		{InstructionName::LAX, AddressingModeName::INX},/*$A3*/
		{InstructionName::LDY, AddressingModeName::ZPI},/*$A4*/
		{InstructionName::LDA, AddressingModeName::ZPI},/*$A5*/
		{InstructionName::LDX, AddressingModeName::ZPI},/*$A6*/
		{InstructionName::LAX, AddressingModeName::ZPI},/*$A7*/
		{InstructionName::TAY, AddressingModeName::IMP},/*$A8*/
		{InstructionName::LDA, AddressingModeName::IMM},/*$A9*/
		{InstructionName::TAX, AddressingModeName::IMP},/*$AA*/
		{InstructionName::LAX, AddressingModeName::IMM},/*$AB*/
		{InstructionName::LDY, AddressingModeName::ABS},/*$AC*/
		{InstructionName::LDA, AddressingModeName::ABS},/*$AD*/
		{InstructionName::LDX, AddressingModeName::ABS},/*$AE*/
		{InstructionName::LAX, AddressingModeName::ABS},/*$AF*/
		{InstructionName::BCS, AddressingModeName::REL},/*$B0*/
		{InstructionName::LDA, AddressingModeName::INY},/*$B1*/
		{InstructionName::STP, AddressingModeName::IMP},/*$B2*/
		{InstructionName::LAX, AddressingModeName::INY},/*$B3*/
		{InstructionName::LDY, AddressingModeName::ZPX},/*$B4*/
		{InstructionName::LDA, AddressingModeName::ZPX},/*$B5*/
		{InstructionName::LDX, AddressingModeName::ZPY},/*$B6*/
		{InstructionName::LAX, AddressingModeName::ZPY},/*$B7*/
		{InstructionName::CLV, AddressingModeName::IMP},/*$B8*/
		{InstructionName::LDA, AddressingModeName::ABY},/*$B9*/
		{InstructionName::TSX, AddressingModeName::IMP},/*$BA*/
		{InstructionName::LAS, AddressingModeName::ABY},/*$BB*/
		{InstructionName::LDY, AddressingModeName::ABX},/*$BC*/
		{InstructionName::LDA, AddressingModeName::ABX},/*$BD*/
		{InstructionName::LDX, AddressingModeName::ABY},/*$BE*/
		{InstructionName::LAX, AddressingModeName::ABY},/*$BF*/
		{InstructionName::CPY, AddressingModeName::IMM},/*$C0*/
		{InstructionName::CMP, AddressingModeName::INX},/*$C1*/
		{InstructionName::DOP, AddressingModeName::IMM},/*$C2*/
		{InstructionName::DCP, AddressingModeName::INX},/*$C3*/
		{InstructionName::CPY, AddressingModeName::ZPI},/*$C4*/
		{InstructionName::CMP, AddressingModeName::ZPI},/*$C5*/
		{InstructionName::DEC, AddressingModeName::ZPI},/*$C6*/
		{InstructionName::DCP, AddressingModeName::ZPI},/*$C7*/
		{InstructionName::INY, AddressingModeName::IMP},/*$C8*/
		{InstructionName::CMP, AddressingModeName::IMM},/*$C9*/
		{InstructionName::DEX, AddressingModeName::IMP},/*$CA*/
		{InstructionName::AXS, AddressingModeName::IMM},/*$CB*/
		{InstructionName::CPY, AddressingModeName::ABS},/*$CC*/
		{InstructionName::CMP, AddressingModeName::ABS},/*$CD*/
		{InstructionName::DEC, AddressingModeName::ABS},/*$CE*/
		{InstructionName::DCP, AddressingModeName::ABS},/*$CF*/
		{InstructionName::BNE, AddressingModeName::REL},/*$D0*/
		{InstructionName::CMP, AddressingModeName::INY},/*$D1*/
		{InstructionName::STP, AddressingModeName::IMP},/*$D2*/
		{InstructionName::DCP, AddressingModeName::INY},/*$D3*/
		{InstructionName::DOP, AddressingModeName::ZPX},/*$D4*/
		{InstructionName::CMP, AddressingModeName::ZPX},/*$D5*/
		{InstructionName::DEC, AddressingModeName::ZPX},/*$D6*/
		{InstructionName::DCP, AddressingModeName::ZPX},/*$D7*/
		{InstructionName::CLD, AddressingModeName::IMP},/*$D8*/
		{InstructionName::CMP, AddressingModeName::ABY},/*$D9*/
		{InstructionName::NOP, AddressingModeName::IMP},/*$DA*/
		{InstructionName::DCP, AddressingModeName::ABY},/*$DB*/
		{InstructionName::TOP, AddressingModeName::ABX},/*$DC*/
		{InstructionName::CMP, AddressingModeName::ABX},/*$DD*/
		{InstructionName::DEC, AddressingModeName::ABX},/*$DE*/
		{InstructionName::DCP, AddressingModeName::ABX},/*$DF*/
		{InstructionName::CPX, AddressingModeName::IMM},/*$E0*/
		{InstructionName::SBC, AddressingModeName::INX},/*$E1*/
		{InstructionName::DOP, AddressingModeName::IMM},/*$E2*/
		{InstructionName::ISC, AddressingModeName::INX},/*$E3*/
		{InstructionName::CPX, AddressingModeName::ZPI},/*$E4*/
		{InstructionName::SBC, AddressingModeName::ZPI},/*$E5*/
		{InstructionName::INC, AddressingModeName::ZPI},/*$E6*/
		{InstructionName::ISC, AddressingModeName::ZPI},/*$E7*/
		{InstructionName::INX, AddressingModeName::IMP},/*$E8*/
		{InstructionName::SBC, AddressingModeName::IMM},/*$E9*/
		{InstructionName::NOP, AddressingModeName::IMP},/*$EA*/
		{InstructionName::SBC, AddressingModeName::IMM},/*$EB*/
		{InstructionName::CPX, AddressingModeName::ABS},/*$EC*/
		{InstructionName::SBC, AddressingModeName::ABS},/*$ED*/
		{InstructionName::INC, AddressingModeName::ABS},/*$EE*/
		{InstructionName::ISC, AddressingModeName::ABS},/*$EF*/
		{InstructionName::BEQ, AddressingModeName::REL},/*$F0*/
		{InstructionName::SBC, AddressingModeName::INY},/*$F1*/
		{InstructionName::STP, AddressingModeName::IMP},/*$F2*/
		{InstructionName::ISC, AddressingModeName::INY},/*$F3*/
		{InstructionName::DOP, AddressingModeName::ZPX},/*$F4*/
		{InstructionName::SBC, AddressingModeName::ZPX},/*$F5*/
		{InstructionName::INC, AddressingModeName::ZPX},/*$F6*/
		{InstructionName::ISC, AddressingModeName::ZPX},/*$F7*/
		{InstructionName::SED, AddressingModeName::IMP},/*$F8*/
		{InstructionName::SBC, AddressingModeName::ABY},/*$F9*/
		{InstructionName::NOP, AddressingModeName::IMP},/*$FA*/
		{InstructionName::ISC, AddressingModeName::ABY},/*$FB*/
		{InstructionName::TOP, AddressingModeName::ABX},/*$FC*/
		{InstructionName::SBC, AddressingModeName::ABX},/*$FD*/
		{InstructionName::INC, AddressingModeName::ABX},/*$FE*/
		{InstructionName::ISC, AddressingModeName::ABX} /*$FF*/

	} };
};
}

#endif