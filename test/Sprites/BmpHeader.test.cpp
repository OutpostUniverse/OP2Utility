#include "../src/Sprites/BmpHeader.h"
#include <gtest/gtest.h>
#include <stdexcept>

TEST(BmpHeader, Create)
{
	const uint32_t fileSize = 100;
	const uint32_t pixelOffset = 50;

	BmpHeader bmpHeader;
	EXPECT_NO_THROW(bmpHeader = BmpHeader::Create(fileSize, pixelOffset));

	EXPECT_EQ(BmpHeader::FileSignature, bmpHeader.fileSignature);
	EXPECT_EQ(fileSize, bmpHeader.size);
	EXPECT_EQ(BmpHeader::DefaultReserved1, bmpHeader.reserved1);
	EXPECT_EQ(BmpHeader::DefaultReserved2, bmpHeader.reserved2);
	EXPECT_EQ(pixelOffset, bmpHeader.pixelOffset);
}

TEST(BmpHeader, IsValidFileSignature) 
{
	BmpHeader bmpHeader;

	bmpHeader.fileSignature = BmpHeader::FileSignature;
	EXPECT_TRUE(bmpHeader.IsValidFileSignature());

	bmpHeader.fileSignature[0] = 'b';
	EXPECT_FALSE(bmpHeader.IsValidFileSignature());
}

TEST(BmpHeader, VerifyFileSignature)
{
	BmpHeader bmpHeader;

	bmpHeader.fileSignature = BmpHeader::FileSignature;
	EXPECT_NO_THROW(bmpHeader.VerifyFileSignature());

	bmpHeader.fileSignature[0] = 'b';
	EXPECT_THROW(bmpHeader.VerifyFileSignature(), std::runtime_error);
}
