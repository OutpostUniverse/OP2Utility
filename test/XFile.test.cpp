#include "../src/XFile.h"
#include <gtest/gtest.h>


TEST(XFile, GetDirectory) {
	// Relative path to file
	EXPECT_EQ("a/", XFile::GetDirectory("a/file.ext"));
	EXPECT_EQ("a/b/", XFile::GetDirectory("a/b/file.ext"));
}
