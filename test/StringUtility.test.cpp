#include "../src/StringUtility.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace StringUtility;

class ImplicitStringConversionTestFixture {
public:
	std::string data;

	operator std::string() const {
		return data;
	}
};

TEST(String, stringFrom) {
	EXPECT_EQ("SomeStringValue", StringFrom("SomeStringValue"));
	EXPECT_EQ("SomeStringValue", StringFrom(std::string{ "SomeStringValue" }));

	EXPECT_EQ("false", StringFrom(false));
	EXPECT_EQ("true", StringFrom(true));

	using signedChar = signed char;
	using unsignedChar = unsigned char;
	EXPECT_EQ("-1", StringFrom(signedChar{ -1 }));
	EXPECT_EQ("0", StringFrom(signedChar{ 0 }));
	EXPECT_EQ("0", StringFrom(unsignedChar{ 0 }));
	EXPECT_EQ("0", StringFrom(char{ 0 }));

	using unsignedShort = unsigned short;
	EXPECT_EQ("-1", StringFrom(short{ -1 }));
	EXPECT_EQ("0", StringFrom(short{ 0 }));
	EXPECT_EQ("0", StringFrom(unsignedShort{ 0 }));

	EXPECT_EQ("-1", StringFrom(-1));
	EXPECT_EQ("0", StringFrom(0));
	EXPECT_EQ("1", StringFrom(1u));

	EXPECT_EQ("-1", StringFrom(-1l));
	EXPECT_EQ("0", StringFrom(0l));
	EXPECT_EQ("1", StringFrom(1ul));

	EXPECT_EQ("-1", StringFrom(-1ll));
	EXPECT_EQ("0", StringFrom(0ll));
	EXPECT_EQ("1", StringFrom(1ull));

	// Ignore precision beyond one decimal place
	EXPECT_THAT(StringFrom(0.0f), testing::StartsWith("0.0"));
	EXPECT_THAT(StringFrom(0.0), testing::StartsWith("0.0"));
	EXPECT_THAT(StringFrom(0.0l), testing::StartsWith("0.0"));

	// Implicit string conversion
	ImplicitStringConversionTestFixture implicitStringConversionTestFixture{ "testString" };
	EXPECT_EQ("testString", StringFrom(implicitStringConversionTestFixture));
}
