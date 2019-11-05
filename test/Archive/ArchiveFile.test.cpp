#include "Archive/VolFile.h"
#include "Archive/ClmFile.h"
#include "XFile.h"
#include <gtest/gtest.h>
#include <vector>
#include <string>

void TestEmptyArchive(Archive::ArchiveFile& archiveFile, const std::string& archiveFilename);


TEST(VolFile, EmptyArchive) 
{
	const std::string archiveFilename("EmptyArchive.vol");

	{
		// Create an empty archive
		const std::vector<std::string> filesToPack;
		Archive::VolFile::CreateArchive(archiveFilename, filesToPack);

		Archive::VolFile archiveFile(archiveFilename);

		SCOPED_TRACE("Empty Volume File");
		TestEmptyArchive(archiveFile, archiveFilename);

		EXPECT_THROW(archiveFile.GetCompressionCode(0), std::runtime_error);
	}

	XFile::DeletePath(archiveFilename);
}

TEST(ClmFile, EmptyArchive) 
{
	const std::string archiveFilename("EmptyArchive.clm");

	{
		// Create an empty archive
		const std::vector<std::string> filesToPack;
		Archive::ClmFile::CreateArchive(archiveFilename, filesToPack);

		Archive::ClmFile archiveFile(archiveFilename);

		SCOPED_TRACE("Empty CLM File");
		TestEmptyArchive(archiveFile, archiveFilename);
	}

	XFile::DeletePath(archiveFilename);
}

// Deletes any files extracted during the test
void TestEmptyArchive(Archive::ArchiveFile& archiveFile, const std::string& archiveFilename)
{
	const std::string extractDirectory("./TestExtract");

	EXPECT_THROW(archiveFile.GetIndex("TestItem"), std::runtime_error);
	EXPECT_THROW(archiveFile.GetName(0), std::runtime_error);
	EXPECT_THROW(archiveFile.GetSize(0), std::runtime_error);
	EXPECT_FALSE(archiveFile.Contains("TestItem"));

	EXPECT_THROW(archiveFile.OpenStream(0), std::runtime_error);
	EXPECT_THROW(archiveFile.ExtractFile(0, "Test"), std::runtime_error);
	EXPECT_NO_THROW(archiveFile.ExtractAllFiles(extractDirectory));

	EXPECT_EQ(archiveFilename, archiveFile.GetArchiveFilename());

	EXPECT_EQ(0u, archiveFile.GetCount());
	EXPECT_LE(0u, archiveFile.GetArchiveFileSize());

	XFile::DeletePath(extractDirectory);
}
