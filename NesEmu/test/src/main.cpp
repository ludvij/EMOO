#include <FileManager/FileManager.hpp>
#include <gtest/gtest.h>

int main(int argc, char** argv)
{
	Fman::PushFolder("EMOO_TEST");
	Fman::SetRoot();

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
