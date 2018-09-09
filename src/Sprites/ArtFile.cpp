#include "ArtFile.h"

const std::array<char, 4> ArtFile::TagPalette{ 'C', 'P', 'A', 'L' };

void ArtFile::ValidateImageMetadata() const
{
	for (const auto& imageMeta : imageMetas) {
		if (imageMeta.scanlineByteWidth != imageMeta.width + (4 - imageMeta.width % 4)) {
			throw std::runtime_error("Image scan line byte width is not valid. It must be the width of the image rounded up to a 4 byte interval.");
		}

		if (imageMeta.paletteIndex > palettes.size()) {
			throw std::runtime_error("Image palette index is out of range of available palettes.");
		}
	}
}

void ArtFile::CountFrames(std::size_t& frameCount, std::size_t& subframeCount, std::size_t& unknownCount) const
{
	frameCount = 0;
	subframeCount = 0;
	unknownCount = 0; //TODO: Figure out what this value is counting. Optional Frame information???

	for (Animation animation : animations) {
		frameCount += animation.frames.size();
		
		for (Animation::Frame frame : animation.frames) {
			subframeCount += frame.subframes.size();
		}
	}
}
