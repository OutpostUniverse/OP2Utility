#include "../src/Sprites/ImageHeader.h"
#include <gtest/gtest.h>
#include <stdexcept>
#include <cstdint>

TEST(ImageHeader, IsBitCount)
{
	for (auto bitCount : ImageHeader::validBitCounts) {
		EXPECT_TRUE(ImageHeader::IsBitCount(bitCount));
	}

	EXPECT_FALSE(ImageHeader::IsBitCount(21));
}

TEST(ImageHeader, IsIndexedBitCount)
{
	for (auto bitCount : ImageHeader::indexedBitCounts) {
		EXPECT_TRUE(ImageHeader::IsIndexedBitCount(bitCount));

		// Ensure Indexed Bit Count is also a valid general BitCount
		EXPECT_TRUE(ImageHeader::IsBitCount(bitCount));
	}

	EXPECT_FALSE(ImageHeader::IsIndexedBitCount(3));
}

TEST(ImageHeader, VerifyBitCount)
{
	for (auto bitCount : ImageHeader::validBitCounts) {
		EXPECT_NO_THROW(ImageHeader::VerifyBitCount(bitCount));
	}
	
	EXPECT_THROW(ImageHeader::VerifyBitCount(21), std::runtime_error);
}

TEST(ImageHeader, VerifyIndexedBitCount) 
{
	for (auto bitCount : ImageHeader::indexedBitCounts) {
		EXPECT_NO_THROW(ImageHeader::VerifyIndexedBitCount(bitCount));

		// Ensure Indexed Bit Count is also a valid general BitCount
		EXPECT_NO_THROW(ImageHeader::VerifyBitCount(bitCount));
	}

	EXPECT_THROW(ImageHeader::VerifyIndexedBitCount(3), std::runtime_error);
}
