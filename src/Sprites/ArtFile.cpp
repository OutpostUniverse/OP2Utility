#include "ArtFile.h"

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
