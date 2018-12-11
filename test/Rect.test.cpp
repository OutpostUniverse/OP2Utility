#include "../src/Rect.h"
#include <gtest/gtest.h>


TEST(Rect, CanComputeWidthAndHeight) {
	const Rect rect1{0, 0, 640, 480};
	EXPECT_EQ(640, rect1.Width());
	EXPECT_EQ(480, rect1.Height());

	const Rect rect2{10, 10, 12, 15};
	EXPECT_EQ(2, rect2.Width());
	EXPECT_EQ(5, rect2.Height());
}
