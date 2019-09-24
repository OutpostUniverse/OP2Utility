#include "../src/XFile.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <regex>


TEST(XFile, HasRootComponent) {
	EXPECT_TRUE(XFile::HasRootComponent("/Path/File.ext"));
#ifdef _WIN32
		// Note: The "C:/" prefix is only considered absolute on Windows
	EXPECT_TRUE(XFile::HasRootComponent("C:/Path/File.ext"));
#endif

	EXPECT_FALSE(XFile::HasRootComponent("Path/File.ext"));
	EXPECT_FALSE(XFile::HasRootComponent("File.ext"));
}

TEST(XFile, MakeAbsolute) {
	EXPECT_EQ("/A/file.ext", XFile::MakeAbsolute("/A/file.ext", "Anything"));
	EXPECT_EQ("/A/file.ext", XFile::MakeAbsolute("A/file.ext", "/"));
	EXPECT_EQ("/A/file.ext", XFile::MakeAbsolute("file.ext", "/A/"));

#ifdef _WIN32
	// Note: The "C:/" prefix is only considered absolute on Windows
	EXPECT_EQ("C:/A/file.ext", XFile::MakeAbsolute("C:/A/file.ext", "Anything"));
#endif
	EXPECT_EQ("C:/A/file.ext", XFile::MakeAbsolute("A/file.ext", "C:/"));
	EXPECT_EQ("C:/A/file.ext", XFile::MakeAbsolute("file.ext", "C:/A/"));

	#ifdef _WIN32
		// For possible future extension on Windows
		// Note: Behavior is currently unimplemented
		// EXPECT_EQ("C:/file.ext", XFile::MakeAbsolute("/file.ext", "C:"));
		// EXPECT_EQ("C:/file.ext", XFile::MakeAbsolute("C:file.ext", "/"));
	#endif
}

TEST(XFile, Append) {
	EXPECT_EQ("a/b/", XFile::Append("a/", "b/"));
	EXPECT_EQ("/a/b/", XFile::Append("/a/", "b/"));

	EXPECT_EQ("C:a/b/", XFile::Append("C:a/", "b/"));
	EXPECT_EQ("C:/a/b/", XFile::Append("C:/a/", "b/"));

	// Make sure we don't try to append a path with a root component
	EXPECT_THROW(XFile::Append("a/", "/b/"), std::runtime_error);
	EXPECT_THROW(XFile::Append("/a/", "/b/"), std::runtime_error);

#ifdef _WIN32
	// Make sure we don't try to append a path with a root component on Windows
	// Note: The right side paths are valid relative paths on Linux, and so are allowed there
	EXPECT_THROW(XFile::Append("a/", "C:b/"), std::runtime_error);
	EXPECT_THROW(XFile::Append("/a/", "C:b/"), std::runtime_error);
	EXPECT_THROW(XFile::Append("a/", "C:/b/"), std::runtime_error);
	EXPECT_THROW(XFile::Append("/a/", "C:/b/"), std::runtime_error);
#endif
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

TEST(XFileGetDirectory, OnlyFile) {
	EXPECT_EQ("", XFile::GetDirectory("file.ext"));
	EXPECT_EQ("", XFile::GetDirectory(".ext"));
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


TEST(XFileGetFilenamesFromDirectory, EmptyPath) {
	EXPECT_THAT(XFile::GetFilenamesFromDirectory(""), testing::Contains(testing::EndsWith("OP2UtilityTest.vcxproj")));
	EXPECT_THAT(XFile::GetFilenamesFromDirectory("", ".vcxproj"), testing::Contains(testing::EndsWith("OP2UtilityTest.vcxproj")));
	EXPECT_THAT(XFile::GetFilenamesFromDirectory("", std::regex(".*[.]vcxproj")), testing::Contains(testing::EndsWith("OP2UtilityTest.vcxproj")));
}

TEST(XFileGetFilenamesFromDirectory, ExplicitCurrentDirectory) {
	EXPECT_THAT(XFile::GetFilenamesFromDirectory("./"), testing::Contains(testing::EndsWith("OP2UtilityTest.vcxproj")));
	EXPECT_THAT(XFile::GetFilenamesFromDirectory("./", ".vcxproj"), testing::Contains(testing::EndsWith("OP2UtilityTest.vcxproj")));
	EXPECT_THAT(XFile::GetFilenamesFromDirectory("./", std::regex(".*[.]vcxproj")), testing::Contains(testing::EndsWith("OP2UtilityTest.vcxproj")));
}
