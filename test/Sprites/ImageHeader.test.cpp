#include "../src/Sprites/ImageHeader.h"
#include <gtest/gtest.h>
#include <stdexcept>
#include <cstdint>

TEST(ImageHeader, IsValidBitCount)
{
	for (auto bitCount : ImageHeader::validBitCounts) {
		EXPECT_TRUE(ImageHeader::IsValidBitCount(bitCount));
	}

	EXPECT_FALSE(ImageHeader::IsValidBitCount(21));
}

TEST(ImageHeader, IsIndexedBitCount)
{
	for (auto bitCount : ImageHeader::indexedBitCounts) {
		EXPECT_TRUE(ImageHeader::IsIndexedBitCount(bitCount));

		// Ensure Indexed Bit Count is also a valid general BitCount
		EXPECT_TRUE(ImageHeader::IsValidBitCount(bitCount));
	}

	EXPECT_FALSE(ImageHeader::IsIndexedBitCount(3));
}

TEST(ImageHeader, VerifyBitCount)
{
	for (auto bitCount : ImageHeader::validBitCounts) {
		EXPECT_NO_THROW(ImageHeader::CheckValidBitCount(bitCount));
	}
	
	EXPECT_THROW(ImageHeader::CheckValidBitCount(21), std::runtime_error);
}

TEST(ImageHeader, VerifyIndexedBitCount) 
{
	for (auto bitCount : ImageHeader::indexedBitCounts) {
		EXPECT_NO_THROW(ImageHeader::CheckIndexedBitCount(bitCount));

		// Ensure Indexed Bit Count is also a valid general BitCount
		EXPECT_NO_THROW(ImageHeader::CheckValidBitCount(bitCount));

	}

	EXPECT_THROW(ImageHeader::CheckIndexedBitCount(3), std::runtime_error);
}
