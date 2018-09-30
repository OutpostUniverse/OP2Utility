#include "../src/Sprites/Color.h"
#include <gtest/gtest.h>

TEST(Color, CompareEquality) {
	// Note: Internally EXPECT_EQ uses `operator ==`
	//       EXPECT_EQ has better error formatting than calling == explicitly

	// Black and white compare equal with themselves
	EXPECT_EQ(DiscreteColor::Black, DiscreteColor::Black);
	EXPECT_EQ(DiscreteColor::White, DiscreteColor::White);
	// Primary colors compare equal with themselves
	EXPECT_EQ(DiscreteColor::Red, DiscreteColor::Red);
	EXPECT_EQ(DiscreteColor::Green, DiscreteColor::Green);
	EXPECT_EQ(DiscreteColor::Blue, DiscreteColor::Blue);

	// Check negative result
	EXPECT_FALSE(DiscreteColor::Black == DiscreteColor::White);
}
