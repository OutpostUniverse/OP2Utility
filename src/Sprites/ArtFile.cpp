#include "ArtFile.h"

Color::Color() : red(0), green(0), blue(0), alpha(0) {}

ImageMeta::ImageMeta() : scanlineByteWidth(0), pixelDataOffset(0), height(0), width(0), type(0), paletteIndex(0) {}

const unsigned int animationUnknown1 = 0x1;
const unsigned int animationUnknown2 = 0x0;

Animation::Animation() : 
	unknown(animationUnknown1), 
	selectionRect(Rect()), 
	pixelXDisplacement(0), 
	pixelYDisplacement(0), 
	unknown2(animationUnknown2) {}

void ArtFile::ValidateImageMetadata() const
{
	for (const auto& imageMeta : imageMetas) {
		//if (imageMeta.scanlineByteWidth % 32 != 0) {
		//	throw std::runtime_error("Image scan line byte width is not divisible by 32.");
		//}

		if (imageMeta.paletteIndex > palettes.size()) {
			throw std::runtime_error("Image palette index is out of range of available palettes.");
		}
	}
}

