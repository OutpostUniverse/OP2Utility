#include "../src/XFile.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <regex>


TEST(XFileGetDirectory, EmptyPath) {
	EXPECT_EQ("", XFile::GetDirectory(""));
}

TEST(XFileGetDirectory, RelativePathToFile) {
	EXPECT_EQ("a/", XFile::GetDirectory("a/file.ext"));
	EXPECT_EQ("a/b/", XFile::GetDirectory("a/b/file.ext"));
}

TEST(XFileGetDirectory, RelativePathToDirectory) {
	EXPECT_EQ("a/", XFile::GetDirectory("a/"));
	EXPECT_EQ("a/b/", XFile::GetDirectory("a/b/"));
}

TEST(XFileGetDirectory, PosixAbsolutePathToFile) {
	EXPECT_EQ("/a/", XFile::GetDirectory("/a/file.ext"));
	EXPECT_EQ("/a/b/", XFile::GetDirectory("/a/b/file.ext"));
}

TEST(XFileGetDirectory, PosixAbsolutePathToDirectory) {
	EXPECT_EQ("/a/", XFile::GetDirectory("/a/"));
	EXPECT_EQ("/a/b/", XFile::GetDirectory("/a/b/"));
}

#ifdef _WIN32
TEST(XFileGetDirectory, WindowsAbsolutePathToFile) {
	EXPECT_EQ("C:/a/", XFile::GetDirectory("C:/a/file.ext"));
	EXPECT_EQ("C:/a/b/", XFile::GetDirectory("C:/a/b/file.ext"));
}

TEST(XFileGetDirectory, WindowsAbsolutePathToDirectory) {
	EXPECT_EQ("C:/a/", XFile::GetDirectory("C:/a/"));
	EXPECT_EQ("C:/a/b/", XFile::GetDirectory("C:/a/b/"));
}
#endif


TEST(XFileGetFilesFromDirectory, EmptyPath) {
	EXPECT_THAT(XFile::GetFilesFromDirectory(""), testing::Contains(testing::EndsWith("OP2UtilityTest.vcxproj")));
	EXPECT_THAT(XFile::GetFilesFromDirectory("", ".vcxproj"), testing::Contains(testing::EndsWith("OP2UtilityTest.vcxproj")));
	EXPECT_THAT(XFile::GetFilesFromDirectory("", std::regex(".*[.]vcxproj")), testing::Contains(testing::EndsWith("OP2UtilityTest.vcxproj")));
}

TEST(XFileGetFilesFromDirectory, ExplicitCurrentDirectory) {
	EXPECT_THAT(XFile::GetFilesFromDirectory("./"), testing::Contains(testing::EndsWith("OP2UtilityTest.vcxproj")));
	EXPECT_THAT(XFile::GetFilesFromDirectory("./", ".vcxproj"), testing::Contains(testing::EndsWith("OP2UtilityTest.vcxproj")));
	EXPECT_THAT(XFile::GetFilesFromDirectory("./", std::regex(".*[.]vcxproj")), testing::Contains(testing::EndsWith("OP2UtilityTest.vcxproj")));
}
