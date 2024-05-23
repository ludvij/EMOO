#include "../TestConfig.hpp"

#include <utils/Disassembler.hpp>

class TestDissasembler : public TestFixture
{
	void SetUp() override
	{
		testing::internal::CaptureStdout();
		console.LoadCartridgeFromMemory(nestest_nes, nestest_nes_len);
		testing::internal::GetCapturedStdout();
	}
};

TEST_F(TestDissasembler, NesTest_Dissasemble)
{
	A6502::Disassembler d;
	d.ConnectBus(&console.GetBus());
	auto res = d.Disassemble();

	ASSERT_EQ(res[console.GetCpu().PC()].repr, "$C004 SEI");
}