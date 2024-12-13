#pragma once
#include "../TestConfig.hpp"

class TestNesTest : public TestFixture
{
	void SetUp() override
	{
		// reload Cartridge memory
		SwitchRom();

		// force reset to get the values I want in the registers
		console.GetCpu().SetPC(0xC000);

	}
};


TEST_F(TestNesTest, RUN_NESTEST)
{
	ASSERT_ANY_THROW(
		while (true)
		{
			console.Step();
			ASSERT_EQ(console.GetBus().Read(0x02), 0) << std::format("Test failed error: [{:02X}]", console.GetBus().Read(0x02));
			ASSERT_EQ(console.GetBus().Read(0x03), 0) << std::format("Test failed error: [{:02X}]", console.GetBus().Read(0x03));
		}
			);
}