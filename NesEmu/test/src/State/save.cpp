
#include "../TestConfig.hpp"

class TestSaveState : public TestFixture
{
};


TEST_F(TestSaveState, SAVE_STATE_DEFAULT)
{
	console.GetBus().Write(12, 23);
	Fman::Serialize(&console.GetBus());
	console.GetBus().Write(12, 0);
	ASSERT_EQ(0, console.GetBus().Peek(12));

	Fman::Deserialize(&console.GetBus());
	ASSERT_EQ(23, console.GetBus().Peek(12));

	Fman::Serialize(&console.GetPpu());

}