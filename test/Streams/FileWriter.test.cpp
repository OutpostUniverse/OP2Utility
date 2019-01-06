#include "Streams/FileWriter.h"
#include "XFile.h"
#include <gtest/gtest.h>


TEST(FileWriterOpenMode, BadFlagCombinations) {
	using OpenMode = Stream::FileWriter::OpenMode;
	const std::string filename("OpenModeBadFlagCombinations.temp");

	// Must specify at least one of CanOpenExisting or CanOpenNew
	EXPECT_THROW(Stream::FileWriter writer(filename, OpenMode::Truncate), std::invalid_argument);
	// Can not specify both Truncate and Append
	OpenMode mode = static_cast<OpenMode>(OpenMode::Truncate | OpenMode::Append);
	EXPECT_THROW(Stream::FileWriter writer(filename, mode), std::invalid_argument);
}

TEST(FileWriterOpenMode, ImplicitDefaultFlags) {
	// Default flags should allow opening new or existing files
	const std::string filename("OpenModeImplicitDefaultFlags.temp");
	EXPECT_NO_THROW(Stream::FileWriter writer(filename));  // File gets created (new)
	EXPECT_NO_THROW(Stream::FileWriter writer(filename));  // File gets re-opened (existing)
	XFile::DeletePath(filename);
}

TEST(FileWriterOpenMode, ExplicitDefaultFlags) {
	using OpenMode = Stream::FileWriter::OpenMode;

	// Default flags includes both CanOpenExisting and CanOpenNew
	ASSERT_TRUE(OpenMode::Default & OpenMode::CanOpenExisting);
	ASSERT_TRUE(OpenMode::Default & OpenMode::CanOpenNew);

	// Open should always succeed when both existing and new files are allowed
	const std::string filename("OpenModeExplicitDefaultFlags.temp");
	ASSERT_NO_THROW(Stream::FileWriter writer(filename, OpenMode::Default));  // File gets created (new)
	ASSERT_NO_THROW(Stream::FileWriter writer(filename, OpenMode::Default));  // File gets re-opened (existing)
	XFile::DeletePath(filename);
}

TEST(FileWriterOpenMode, PermissionChecks) {
	using OpenMode = Stream::FileWriter::OpenMode;
	std::string filename("OpenModePermissionChecks.temp");

	// Try to open new file without permission to create new files
	ASSERT_THROW(Stream::FileWriter writer(filename, OpenMode::CanOpenExisting), std::runtime_error);

	// Create new file, and then re-open existing file
	EXPECT_NO_THROW(Stream::FileWriter writer(filename, OpenMode::CanOpenNew));
	EXPECT_NO_THROW(Stream::FileWriter writer(filename, OpenMode::CanOpenExisting));

	// Try to open existing file without permission for existing files
	ASSERT_THROW(Stream::FileWriter writer(filename, OpenMode::CanOpenNew), std::runtime_error);

	// Cleanup temporary file
	XFile::DeletePath(filename);
}
