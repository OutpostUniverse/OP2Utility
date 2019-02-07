#include "ArtFile.h"
#include <stdexcept>


constexpr Tag ArtFile::TagPalette;


void ArtFile::VerifyImageIndexInBounds(std::size_t index)
{
	if (index > imageMetas.size()) {
		throw std::runtime_error("An index of " + std::to_string(index) + " exceeds range of images");
	}
}

void ArtFile::ValidateImageMetadata() const
{
	for (const auto& imageMeta : imageMetas) {
		// Bitwise operation rounds up to the next 4 byte interval
		if (imageMeta.scanLineByteWidth != ( (imageMeta.width + 3) & ~3) ) {
			throw std::runtime_error("Image scan line byte width is not valid. It must be the width of the image rounded up to a 4 byte interval.");
		}

		if (imageMeta.paletteIndex >= palettes.size()) {
			throw std::runtime_error("Image palette index is out of range of available palettes.");
		}
	}
}

void ArtFile::CountFrames(std::size_t& frameCount, std::size_t& layerCount, std::size_t& unknownCount) const
{
	frameCount = 0;
	layerCount = 0;
	unknownCount = unknownAnimationCount; //TODO: Figure out what this value is counting. Optional Frame information???

	for (Animation animation : animations) {
		frameCount += animation.frames.size();
		
		for (Animation::Frame frame : animation.frames) {
			layerCount += frame.layers.size();
		}
	}
}
