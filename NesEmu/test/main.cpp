#include "pch.hpp"
#ifdef NES_EMU_TEST


#include <iostream>

#include <gtest/gtest.h>



int main( int argc, char **argv ) {
	testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}

#endif