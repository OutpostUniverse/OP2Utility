#include "../src/XFile.h"
#include <gtest/gtest.h>

TEST(XFile, RemoveFilename)
{
	EXPECT_EQ(XFile::RemoveFilename("Ashes.map"), "./");
	EXPECT_EQ(XFile::RemoveFilename("./Ashes.map"), "./");
	EXPECT_EQ(XFile::RemoveFilename("C:/Ashes.map"), "C:/");
	EXPECT_EQ(XFile::RemoveFilename("C:/Directory/Ashes.map"), "C:/Directory/");
	EXPECT_EQ(XFile::RemoveFilename("../Ashes.map"), "../");
	EXPECT_EQ(XFile::RemoveFilename("Directory/Ashes.map"), "Directory/");
	EXPECT_EQ(XFile::RemoveFilename("./Directory/Ashes.map"), "./Directory/");
}
