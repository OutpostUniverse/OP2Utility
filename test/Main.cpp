#ifdef _WIN32

#include "gtest/gtest.h"

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

#endif
