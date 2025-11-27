#include <FileManager/FileManager.hpp>
#include <gtest/gtest.h>

int main(int argc, char** argv)
{
	varf::PushFolder("EMOO_TEST");
	varf::SetRoot();

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
