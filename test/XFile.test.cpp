#include "../src/XFile.h"
#include <gtest/gtest.h>

// Disable due to lack of Google Mock in the Windows CI environment
#ifndef _WIN32
#include <gmock/gmock.h>
#include <regex>
#endif


TEST(XFile, Append) {
	EXPECT_EQ("a/b/", XFile::Append("a/", "b/"));
	EXPECT_EQ("/a/b/", XFile::Append("/a/", "b/"));

	// EXPECT_THROW(XFile::Append("a/", "/b/"), std::runtime_error);
	// EXPECT_THROW(XFile::Append("/a/", "/b/"), std::runtime_error);
}

TEST(XFileReplaceFilename, ReplaceFilename) {
	EXPECT_EQ(XFile::ReplaceFilename("./Old.map", "New.map"), "./New.map");
	EXPECT_EQ(XFile::ReplaceFilename("Old.map", "New.map"), "New.map");
	EXPECT_EQ(XFile::ReplaceFilename("C:/Old.map", "New.map"), "C:/New.map");
	EXPECT_EQ(XFile::ReplaceFilename("C:/Directory/Old.map", "New.map"), "C:/Directory/New.map");
	EXPECT_EQ(XFile::ReplaceFilename("../Old.map", "New.map"), "../New.map");
	EXPECT_EQ(XFile::ReplaceFilename("Directory/Old.map", "New.map"), "Directory/New.map");
	EXPECT_EQ(XFile::ReplaceFilename("./Directory/Old.map", "New.map"), "./Directory/New.map");
	EXPECT_EQ(XFile::ReplaceFilename("Old Space.map", "New Space.map"), "New Space.map");
}

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

// The follow two tests use paths that only apply to Windows (yet still pass on Linux)

TEST(XFileGetDirectory, WindowsAbsolutePathToFile) {
	EXPECT_EQ("C:/a/", XFile::GetDirectory("C:/a/file.ext"));
	EXPECT_EQ("C:/a/b/", XFile::GetDirectory("C:/a/b/file.ext"));
}

TEST(XFileGetDirectory, WindowsAbsolutePathToDirectory) {
	EXPECT_EQ("C:/a/", XFile::GetDirectory("C:/a/"));
	EXPECT_EQ("C:/a/b/", XFile::GetDirectory("C:/a/b/"));
}


// Disable due to lack of Google Mock in the Windows CI environment
#ifndef _WIN32
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
#endif
