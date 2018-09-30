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

TEST(Color, CompareEqualityWithNewlyConstructedValue) {
	// Construct these non-inline so as to not interfere with macro expansion
	// (Preprocessor gets confused when C++ grammar ","s appear on the same line)
	const Color Black{ 0, 0, 0, 0 };
	const Color White{ 255, 255, 255, 0 };
	// Colors compare equal with constructed equivalents
	EXPECT_EQ(DiscreteColor::Black, Black);
	EXPECT_EQ(DiscreteColor::White, White);
}

TEST(Color, CompareNotEqual) {
	// Note: Internally EXPECT_NE check `operator !=`
	//       EXPECT_NE has better error formatting than calling != explicitly

	// Black and white are distinct (3 components differ)
	EXPECT_NE(DiscreteColor::Black, DiscreteColor::White);
	EXPECT_NE(DiscreteColor::White, DiscreteColor::Black);

	// Primary colors are distinct (2 components differ)
	EXPECT_NE(DiscreteColor::Red, DiscreteColor::Green);
	EXPECT_NE(DiscreteColor::Red, DiscreteColor::Blue);
	EXPECT_NE(DiscreteColor::Green, DiscreteColor::Blue);
	EXPECT_NE(DiscreteColor::Green, DiscreteColor::Red);
	EXPECT_NE(DiscreteColor::Blue, DiscreteColor::Red);
	EXPECT_NE(DiscreteColor::Blue, DiscreteColor::Green);

	// Primary and Black differ (1 component differs)
	EXPECT_NE(DiscreteColor::Red, DiscreteColor::Black);
	EXPECT_NE(DiscreteColor::Green, DiscreteColor::Black);
	EXPECT_NE(DiscreteColor::Blue, DiscreteColor::Black);
}
