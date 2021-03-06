#include "../src/Rect.h"
#include <gtest/gtest.h>

using namespace OP2Utility;

TEST(Rect, CanComputeWidthAndHeight) {
	const Rect rect1{0, 0, 640, 480};
	EXPECT_EQ(640, rect1.Width());
	EXPECT_EQ(480, rect1.Height());

	const Rect rect2{10, 10, 12, 15};
	EXPECT_EQ(2, rect2.Width());
	EXPECT_EQ(5, rect2.Height());
}

TEST(Rect, ComparisonOperators) {
	const Rect rect1{0, 0, 640, 480};
	const Rect rect2{0, 0, 640, 480};
	const Rect rect3{10, 10, 12, 15};

	// Note: Internally EXPECT_EQ uses `operator ==`
	EXPECT_EQ(rect1, rect1);
	EXPECT_EQ(rect1, rect2);
	EXPECT_EQ(rect2, rect1);
	EXPECT_EQ(rect2, rect2);
	// Note: Internally EXPECT_NE uses `operator !=`
	EXPECT_NE(rect1, rect3);
	EXPECT_NE(rect2, rect3);
}
