#pragma once
#include "../TestConfig.hpp"

class TestNesTest : public TestFixture
{
	void SetUp() override
	{
		// reload Cartridge memory
		console.LoadCartridgeFromMemory(nestest_nes, nestest_nes_len);


		// force reset to get the values I want in the registers
		console.GetCpu().SetPC(0xC000);

	}
};


TEST_F(TestNesTest, RunNesTest)
{
	try
	{

		while (true)
		{
			console.Step();
			ASSERT_EQ(console.GetBus().Read(0x02), 0) << std::format("Test failed error: [{:02X}]", console.GetBus().Read(0x02));
			ASSERT_EQ(console.GetBus().Read(0x03), 0) << std::format("Test failed error: [{:02X}]", console.GetBus().Read(0x03));
		}
	} catch (const std::runtime_error&)
	{

	}
}