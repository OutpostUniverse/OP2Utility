#include "../src/ResourceManager.h"
#include <gtest/gtest.h>


TEST(ResourceManager, GetResourceStream) {
	ResourceManager resourceManager("");

	// Refuses absolute paths
#ifdef _WIN32
	EXPECT_THROW(resourceManager.GetResourceStream("C:/Archive.vol"), std::runtime_error);
	EXPECT_THROW(resourceManager.GetResourceStream("C:/PathTo/Archive.vol"), std::runtime_error);
#endif
	EXPECT_THROW(resourceManager.GetResourceStream("/Archive.vol"), std::runtime_error);
	EXPECT_THROW(resourceManager.GetResourceStream("/PathTo/Archive.vol"), std::runtime_error);
}
