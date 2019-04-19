#include "../src/ResourceManager.h"
#include <gtest/gtest.h>
#include <stdexcept>

TEST(ResourceManager, ConstructResourceManager)
{
	// Load Resource Manager where directories exist with .vol and .clm 'extensions'
	EXPECT_NO_THROW(ResourceManager("./data"));

	// Refuse to load when passed a filename
	EXPECT_THROW(ResourceManager("./data/Directory.clm/.keep"), std::runtime_error);
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
