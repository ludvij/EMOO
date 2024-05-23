#include "../TestConfig.hpp"

#include <utils/Disassembler.hpp>

class TestDisassembler : public TestFixture
{
	//void SetUp() override
	//{
	//	testing::internal::CaptureStdout();
	//	console.LoadCartridgeFromMemory(nestest_nes, nestest_nes_len);
	//	testing::internal::GetCapturedStdout();
	//}
};

TEST_F(TestDisassembler, Assemble_Disassemble)
{
	asse.Assemble(R"(
		SEI
		CLD
		LDX #$FF
		TXS
		LDA $2002
		BPL -5
		JMP $0001
	)");

	A6502::Disassembler d;
	d.ConnectBus(&console.GetBus());
	auto res = d.Disassemble();

	std::map<u16, A6502::Disassembly> test{
		{0x0000, {"SEI", "program_start"}},
		{0x0001, {"CLD", "_label_0001"}},
		{0x0002, {"LDX #$FF", ""}},
		{0x0004, {"TXS", ""}},
		{0x0005, {"LDA PPU_STATUS", "_label_0000"}},
		{0x0008, {"BPL _label_0000", ""}},
		{0x000A, {"JMP _label_0001", ""}},
	};

	for (const auto& [k, v] : res | std::views::take(6))
	{
		ASSERT_EQ(test[k].repr, v.repr);
		ASSERT_EQ(test[k].label, v.label);
	}
}