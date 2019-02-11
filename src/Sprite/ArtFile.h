#pragma once

#include "ImageMeta.h"
#include "Animation.h"
#include "../Bitmap/Color.h"
#include "../Tag.h"
#include <vector>
#include <array>
#include <string>
#include <cstddef>

namespace Stream {
	class BidirectionalSeekableReader;
	class BidirectionalSeekableWriter;
}

struct ArtFile
{
public:
	std::vector<Palette> palettes;
	std::vector<ImageMeta> imageMetas;
	std::vector<Animation> animations;
	uint32_t unknownAnimationCount;

	static ArtFile Read(std::string filename);
	static ArtFile Read(Stream::BidirectionalSeekableReader& seekableReader);
	static void Write(std::string filename, const ArtFile& artFile);
	static void Write(Stream::BidirectionalSeekableWriter&, const ArtFile& artFile);

	void VerifyImageIndexInBounds(std::size_t index);

private:
	// Read Functions
	static void ReadPalette(Stream::BidirectionalSeekableReader& seekableReader, ArtFile& artFile);
	static void ReadImageMetadata(Stream::BidirectionalSeekableReader& seekableReader, ArtFile& artFile);
	static void ReadAnimations(Stream::BidirectionalSeekableReader& seekableReader, ArtFile& artFile);
	static Animation ReadAnimation(Stream::BidirectionalSeekableReader& seekableReader);
	static Animation::Frame ReadFrame(Stream::BidirectionalSeekableReader& seekableReader);
	static void VerifyCountsMatchHeader(const ArtFile& artFile, std::size_t frameCount, std::size_t layerCount, std::size_t unknownCount);


	// Write Functions
	static void WritePalettes(Stream::BidirectionalSeekableWriter& seekableWriter, const ArtFile& artFile);
	static void WriteAnimations(Stream::BidirectionalSeekableWriter& seekableWriter, const ArtFile& artFile);
	static void WriteAnimation(Stream::BidirectionalSeekableWriter& seekableWriter, const Animation& animation);
	static void WriteFrame(Stream::BidirectionalSeekableWriter& seekableWriter, const Animation::Frame& frame);


	void ValidateImageMetadata() const;
	void CountFrames(std::size_t& frameCount, std::size_t& layerCount, std::size_t& unknownCount) const;

	static constexpr auto TagPalette = MakeTag("CPAL");
};
