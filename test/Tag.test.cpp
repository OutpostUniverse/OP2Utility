#include "../src/Tag.h"
#include <gtest/gtest.h>
#include <sstream>
#include <type_traits>


TEST(Tag, RemoveLastElement) {
	// Note: Empty string (size 0) omitted, since variables may always take at least 1 byte
	EXPECT_EQ(1u, sizeof(RemoveLastElement("A")));
	EXPECT_EQ(4u, sizeof(RemoveLastElement("volh")));
	EXPECT_EQ(11u, sizeof(RemoveLastElement("some string")));

	// Wide string literals and unicode string literals
	EXPECT_EQ(4u, RemoveLastElement(L"wide").size());
	EXPECT_EQ(5u, RemoveLastElement(u8"UTF-8").size());
	EXPECT_EQ(6u, RemoveLastElement(u"UTF-16").size());
	EXPECT_EQ(6u, RemoveLastElement(U"UTF-32").size());
}

TEST(Tag, Properties) {
	// Proper size
	EXPECT_EQ(4u, sizeof(Tag));

	// Tags can be memcopied
	EXPECT_TRUE(std::is_trivially_copyable<Tag>::value);

	// Default constructible (can be declared or embedded in a struct without initialization)
	// Example:  Tag defaultTag;
	EXPECT_TRUE(std::is_default_constructible<Tag>::value);

	// Copy constructible (copy one tag to another)
	EXPECT_TRUE(std::is_copy_constructible<Tag>::value);

	// Assignable
	EXPECT_TRUE((std::is_assignable<Tag, Tag>::value));
	EXPECT_TRUE((std::is_assignable<Tag, const char[5]>::value));
}

TEST(Tag, MakeTag) {
	auto tag1 = MakeTag("VOL ");
	auto tag2 = MakeTag("VOL ");
	auto tag3 = MakeTag("volh");

	// Equality and inequality comparable
	EXPECT_EQ(tag1, tag2);
	EXPECT_NE(tag1, tag3);

	// Convertible to std::string
	std::string string = tag1;
	EXPECT_EQ("VOL ", string);

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
