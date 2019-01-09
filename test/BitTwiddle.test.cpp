#include "../src/BitTwiddle.h"
#include <gtest/gtest.h>


TEST(BitTwiddle, IsPowerOf2) {
	EXPECT_TRUE(IsPowerOf2(1));
	EXPECT_TRUE(IsPowerOf2(2));
	EXPECT_TRUE(IsPowerOf2(4));
	EXPECT_TRUE(IsPowerOf2(8));
	EXPECT_TRUE(IsPowerOf2(16));
	EXPECT_TRUE(IsPowerOf2(32));
	EXPECT_TRUE(IsPowerOf2(64));
	EXPECT_TRUE(IsPowerOf2(128));
	EXPECT_TRUE(IsPowerOf2(256));
	EXPECT_TRUE(IsPowerOf2(512));

	EXPECT_TRUE(IsPowerOf2(1 << 31));

	EXPECT_FALSE(IsPowerOf2(0));
	EXPECT_FALSE(IsPowerOf2(3));
	EXPECT_FALSE(IsPowerOf2(5));
	EXPECT_FALSE(IsPowerOf2(6));
	EXPECT_FALSE(IsPowerOf2(7));
}

TEST(BitTwiddle, Log2OfPowerOf2) {
	EXPECT_EQ(0, Log2OfPowerOf2(1));
	EXPECT_EQ(1, Log2OfPowerOf2(2));
	EXPECT_EQ(2, Log2OfPowerOf2(4));
	EXPECT_EQ(3, Log2OfPowerOf2(8));
	EXPECT_EQ(4, Log2OfPowerOf2(16));
	EXPECT_EQ(5, Log2OfPowerOf2(32));
	EXPECT_EQ(6, Log2OfPowerOf2(64));
	EXPECT_EQ(7, Log2OfPowerOf2(128));
	EXPECT_EQ(8, Log2OfPowerOf2(256));
	EXPECT_EQ(9, Log2OfPowerOf2(512));

	EXPECT_EQ(31, Log2OfPowerOf2(1 << 31));
}
