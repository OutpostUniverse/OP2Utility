#include "../src/Rect.h"
#include <gtest/gtest.h>

class SimpleRect : public ::testing::Test {
public:
	SimpleRect() : rect(Rect{ x1, y1, x2, y2 }) {}

protected:
	const int x1 = 0;
	const int y1 = 0;
	const int x2 = 10;
	const int y2 = 10;

	Rect rect;
};

TEST_F(SimpleRect, Construction)
{
	EXPECT_EQ(x1, rect.x1);
	EXPECT_EQ(y1, rect.y1);

	EXPECT_EQ(x2, rect.x2);
	EXPECT_EQ(y2, rect.y2);
}

TEST_F(SimpleRect, Width) {
	EXPECT_EQ(x2 - x1, rect.Width());
}

TEST_F(SimpleRect, Height)
{
	EXPECT_EQ(y2 - y1, rect.Height());
}


TEST(Rect, CanComputeWidthAndHeight) {
	const Rect rect1{0, 0, 640, 480};
	EXPECT_EQ(640, rect1.Width());
	EXPECT_EQ(480, rect1.Height());

	const Rect rect2{10, 10, 12, 15};
	EXPECT_EQ(2, rect2.Width());
	EXPECT_EQ(5, rect2.Height());
}
