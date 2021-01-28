#include "../src/ResourceManager.h"
#include "../src/Archive/VolFile.h"
#include "../src/XFile.h"
#include <gtest/gtest.h>
#include <stdexcept>

using namespace OP2Utility;

TEST(ResourceManager, ConstructResourceManager)
{
	// Load Resource Manager where directories exist with .vol and .clm 'extensions'
	EXPECT_NO_THROW(ResourceManager("./data"));

	// Refuse to load when passed a filename
	EXPECT_THROW(ResourceManager("./data/Empty.txt"), std::runtime_error);
}

TEST(ResourceManager, GetResourceStream_RefuseAbsolutePath) {
	ResourceManager resourceManager("");

#ifdef _WIN32
	EXPECT_THROW(resourceManager.GetResourceStream("C:/Archive.vol"), std::runtime_error);
	EXPECT_THROW(resourceManager.GetResourceStream("C:/PathTo/Archive.vol"), std::runtime_error);
#endif
	EXPECT_THROW(resourceManager.GetResourceStream("/Archive.vol"), std::runtime_error);
	EXPECT_THROW(resourceManager.GetResourceStream("/PathTo/Archive.vol"), std::runtime_error);
}

TEST(ResourceManager, GetResourceStreamExistingFileUnpacked)
{
	ResourceManager resourceManager("./data");
	// Opening an existing file should result in a valid stream
	EXPECT_NE(nullptr, resourceManager.GetResourceStream("Empty.txt"));
}

TEST(ResourceManager, GetFilenames)
{
	// Ensure Directory.vol is not returned
	ResourceManager resourceManager("./data");
	auto filenames = resourceManager.GetAllFilenamesOfType(".vol");

	EXPECT_EQ(0u, filenames.size());

	EXPECT_EQ(0u, resourceManager.GetAllFilenames("Directory.vol").size());
}

TEST(ResourceManager, GetArchiveFilenames)
{
	const std::string archiveName("./data/Test.vol");
	Archive::VolFile::CreateArchive(archiveName, {});

	// Scope block to ensures ResourceManager is destructed after use
	// This ensures the VOL file is closed before attempting to delete it
	// This is needed for Windows filesystem semantics
	{
		ResourceManager resourceManager("./data");
		EXPECT_EQ(std::vector<std::string>{archiveName}, resourceManager.GetArchiveFilenames());
	}

	XFile::DeletePath(archiveName);
}
