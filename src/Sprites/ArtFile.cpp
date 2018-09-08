#include "ArtFile.h"

const unsigned int animationUnknown1 = 0x1;
const unsigned int animationUnknown2 = 0x0;

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

