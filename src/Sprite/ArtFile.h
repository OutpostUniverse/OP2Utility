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
	class Reader;
	class BidirectionalWriter;
}

struct ArtFile
{
public:
	std::vector<Palette> palettes;
	std::vector<ImageMeta> imageMetas;
	std::vector<Animation> animations;
	uint32_t unknownAnimationCount;

	static ArtFile Read(std::string filename);
	static ArtFile Read(Stream::Reader& reader);
	static void Write(std::string filename, const ArtFile& artFile);
	static void Write(Stream::BidirectionalWriter&, const ArtFile& artFile);

	void VerifyImageIndexInBounds(std::size_t index);

private:
	// Read Functions
	static void ReadPalette(Stream::Reader& reader, ArtFile& artFile);
	static void ReadImageMetadata(Stream::Reader& reader, ArtFile& artFile);
	static void ReadAnimations(Stream::Reader& reader, ArtFile& artFile);
	static Animation ReadAnimation(Stream::Reader& reader);
	static Animation::Frame ReadFrame(Stream::Reader& reader);
	static void VerifyCountsMatchHeader(const ArtFile& artFile, std::size_t frameCount, std::size_t layerCount, std::size_t unknownCount);


	// Write Functions
	static void WritePalettes(Stream::BidirectionalWriter& seekableWriter, const ArtFile& artFile);
	static void WriteAnimations(Stream::BidirectionalWriter& seekableWriter, const ArtFile& artFile);
	static void WriteAnimation(Stream::BidirectionalWriter& seekableWriter, const Animation& animation);
	static void WriteFrame(Stream::BidirectionalWriter& seekableWriter, const Animation::Frame& frame);


	void ValidateImageMetadata() const;
	void CountFrames(std::size_t& frameCount, std::size_t& layerCount, std::size_t& unknownCount) const;

	static constexpr auto TagPalette = MakeTag("CPAL");
};
