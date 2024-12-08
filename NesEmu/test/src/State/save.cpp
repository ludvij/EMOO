
#include "../TestConfig.hpp"

class TestSaveState : public TestFixture
{
};


TEST_F(TestSaveState, SAVE_STATE_DEFAULT)
{
	console.GetBus().Write(12, 23);
	console.SaveState(0);
	console.GetBus().Write(12, 0);
	ASSERT_EQ(0, console.GetBus().Peek(12));

	// check saving
	console.LoadState(0);
	ASSERT_EQ(23, console.GetBus().Peek(12));

	// check loading
	console.GetBus().Write(12, 47);
	console.SaveState(1);
	console.GetBus().Write(12, 1);
	ASSERT_EQ(1, console.GetBus().Peek(12));

	// check that multiple states coexist
	console.LoadState(1);
	ASSERT_EQ(47, console.GetBus().Peek(12));
	console.LoadState(0);
	ASSERT_EQ(23, console.GetBus().Peek(12));
}

TEST_F(TestSaveState, SAVE_STATE_MULTIPLE_SAME_ROM)
{
	console.GetBus().Write(12, 23);
	console.SaveState(0);


	console.GetBus().Write(12, 47);
	console.SaveState(1);

	// check that multiple states coexist
	console.LoadState(0);
	ASSERT_EQ(23, console.GetBus().Peek(12));
	console.LoadState(1);
	ASSERT_EQ(47, console.GetBus().Peek(12));
}

TEST_F(TestSaveState, SAVE_TEST_LOAD_EMPTY)
{
	console.GetBus().Write(12, 23);
	console.SaveState(0);

	// check that multiple states coexist
	console.LoadState(2);
	ASSERT_EQ(23, console.GetBus().Peek(12));
}

TEST_F(TestSaveState, SAVE_STATE_MULTIPLE_ROM)
{
	console.GetBus().Write(12, 23);
	console.SaveState(0);


	console.GetBus().Write(12, 47);
	console.SaveState(1);

	SwitchRom();

	ASSERT_EQ(0, console.GetBus().Peek(12));
	console.GetBus().Write(12, 56);
	console.SaveState(0);

	SwitchRom();
	ASSERT_EQ(0, console.GetBus().Peek(12));

	console.LoadState(0);
	ASSERT_EQ(23, console.GetBus().Peek(12));
	console.LoadState(1);
	ASSERT_EQ(47, console.GetBus().Peek(12));

	SwitchRom();
	console.LoadState(0);
	ASSERT_EQ(56, console.GetBus().Peek(12));

}
