#include "BitmapFile.h"
#include <stdexcept>
#include <cmath>

void BitmapFile::VerifyPixelSizeMatchesImageDimensionsWithPitch(uint16_t bitCount, int32_t width, int32_t height, std::size_t pixelsWithPitchSize)
{
	if (pixelsWithPitchSize != ImageHeader::CalculatePitch(bitCount, width) * std::abs(height)) {
		throw std::runtime_error("The size of pixels does not match the image's height time pitch");
	}
}
