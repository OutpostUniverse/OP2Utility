#include "../src/Tag.h"
#include <gtest/gtest.h>
#include <sstream>


TEST(Tag, RemoveLastElement) {
	// Note: Empty string (size 0) omitted, since variables may always take at least 1 byte
	EXPECT_EQ(1, sizeof(RemoveLastElement("A")));
	EXPECT_EQ(4, sizeof(RemoveLastElement("volh")));
	EXPECT_EQ(11, sizeof(RemoveLastElement("some string")));

	// Wide string literals and unicode string literals
	EXPECT_EQ(4, RemoveLastElement(L"wide").size());
	EXPECT_EQ(5, RemoveLastElement(u8"UTF-8").size());
	EXPECT_EQ(6, RemoveLastElement(u"UTF-16").size());
	EXPECT_EQ(6, RemoveLastElement(U"UTF-32").size());
}

TEST(Tag, MakeTag) {
	auto tag1 = MakeTag("VOL ");
	auto tag2 = MakeTag("VOL ");
	auto tag3 = MakeTag("volh");

	// Proper size
	EXPECT_EQ(4, sizeof(tag1));

	// Equality and inequality comparable
	EXPECT_EQ(tag1, tag2);
	EXPECT_NE(tag1, tag3);

	// Concatenation with string literals
	std::string appendString1 = "String literal: " + tag1;
	EXPECT_EQ("String literal: VOL ", appendString1);

	// Concatenation with std::string
	std::string appendString2 = std::string("std::string: ") + tag1;
	EXPECT_EQ("std::string: VOL ", appendString2);

	// Printable with standard serialization
	std::ostringstream stream;
	stream << tag1;
	EXPECT_EQ("VOL ", stream.str());
}
