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
	.reset $8000
	.at $8000
		SEI
		CLD
		LDX #$FF
		TXS
		LDA $2002
		BPL -5
		JMP $0001
	)");


	console.Reset();

	A6502::Disassembler d;
	d.ConnectBus(&console.GetBus());
	d.Init();

	const auto pc = console.GetCpu().PC();

	std::map<u16, A6502::Disassembly> test{
		{static_cast<u16>( pc + 0x0000 ), {"SEI", "Reset"}},
		{static_cast<u16>( pc + 0x0001 ), {"CLD", ""}},
		{static_cast<u16>( pc + 0x0002 ), {"LDX #$FF", ""}},
		{static_cast<u16>( pc + 0x0004 ), {"TXS", ""}},
		{static_cast<u16>( pc + 0x0005 ), {"LDA PPU_STATUS", "_label_0000"}},
		{static_cast<u16>( pc + 0x0008 ), {"BPL _label_0000", ""}},
	};

	for (const auto& [k, v] : test)
	{
		ASSERT_TRUE(d.Contains(k)) << std::format("Missing pc value: {:04X}", k);
		ASSERT_EQ(d.Get(k).repr, v.repr);
		ASSERT_EQ(d.Get(k).label, v.label);
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
	d.Init();
	const auto pc = console.GetCpu().PC();


	std::map<u16, A6502::Disassembly> test{
		{static_cast<u16>( pc + 0x0000 ), {"SEI", "Reset"}},
		{static_cast<u16>( pc + 0x0001 ), {"CLD", ""}},
		{static_cast<u16>( pc + 0x0002 ), {"LDX #$FF", ""}},
		{static_cast<u16>( pc + 0x0004 ), {"TXS", ""}},
		{static_cast<u16>( pc + 0x0005 ), {"LDA PPU_STATUS", "_label_0000"}},
		{static_cast<u16>( pc + 0x0008 ), {"BPL _label_0000", ""}},
	};

	for (const auto& [k, v] : test)
	{
		ASSERT_TRUE(d.Contains(k)) << std::format("Missing pc value: {:04X}", k);
		ASSERT_EQ(d.Get(k).repr, v.repr);
		ASSERT_EQ(d.Get(k).label, v.label);
	}
}