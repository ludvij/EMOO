#include "../TestConfig.hpp"

#include <utils/Disassembler.hpp>

class TestDisassembler : public TestFixture
{
	void SetUp() override
	{
		testing::internal::CaptureStdout();
		console.LoadCartridgeFromMemory(nestest_nes, nestest_nes_len);
		testing::internal::GetCapturedStdout();
	}
public:
	void SwitchRom()
	{
		if (!switched)
		{
			testing::internal::CaptureStdout();
			console.LoadCartridgeFromMemory(blank_nes, blank_nes_len);
			testing::internal::GetCapturedStdout();
		}
		else
		{
			testing::internal::CaptureStdout();
			console.LoadCartridgeFromMemory(nestest_nes, nestest_nes_len);
			testing::internal::GetCapturedStdout();
		}

		switched = !switched;
	}

	bool switched{ false };
};

TEST_F(TestDisassembler, AssembleDisassemble)
{
	SwitchRom();

	asse.Assemble(R"(
		%start $8000
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

	const auto pc = console.GetCpu().PC();

	std::map<u16, A6502::Disassembly> test{
		{pc + 0x0000, {"SEI", "Reset"}},
		{pc + 0x0001, {"CLD", ""}},
		{pc + 0x0002, {"LDX #$FF", ""}},
		{pc + 0x0004, {"TXS", ""}},
		{pc + 0x0005, {"LDA PPU_STATUS", "_label_021F"}},
		{pc + 0x0008, {"BPL _label_021F", ""}},
	};

	for (const auto& [k, v] : test)
	{
		ASSERT_TRUE(res.contains(k)) << std::format("Missing pc value: {:04X}", k);
		ASSERT_EQ(res[k].repr, v.repr);
		ASSERT_EQ(res[k].label, v.label);
	}
}

TEST_F(TestDisassembler, DisassembleNesTest)
{
	/*asse.Assemble(R"(
		SEI
		CLD
		LDX #$FF
		TXS
		LDA $2002
		BPL -5
		JMP $0001
	)");*/

	A6502::Disassembler d;
	d.ConnectBus(&console.GetBus());
	auto res = d.Disassemble();

	const auto pc = console.GetCpu().PC();

	std::map<u16, A6502::Disassembly> test{
		{pc + 0x0000, {"SEI", "Reset"}},
		{pc + 0x0001, {"CLD", ""}},
		{pc + 0x0002, {"LDX #$FF", ""}},
		{pc + 0x0004, {"TXS", ""}},
		{pc + 0x0005, {"LDA PPU_STATUS", "_label_021F"}},
		{pc + 0x0008, {"BPL _label_021F", ""}},
	};

	for (const auto& [k, v] : test)
	{
		ASSERT_TRUE(res.contains(k)) << std::format("Missing pc value: {:04X}", k);
		ASSERT_EQ(res[k].repr, v.repr);
		ASSERT_EQ(res[k].label, v.label);
	}
}