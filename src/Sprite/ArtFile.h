#pragma once

#include "ImageMeta.h"
#include "Animation.h"
#include "../Bitmap/Color.h"
#include <vector>
#include <array>
#include <string>
#include <cstddef>

namespace Stream {
	class BiDirectionalSeekableReader;
	class BiDirectionalSeekableWriter;
}

struct ArtFile
{
public:
	std::vector<Palette> palettes;
	std::vector<ImageMeta> imageMetas;
	std::vector<Animation> animations;
	uint32_t unknownAnimationCount;

	static ArtFile Read(std::string filename);
	static ArtFile Read(Stream::BiDirectionalSeekableReader& seekableReader);
	static void Write(std::string filename, const ArtFile& artFile);
	static void Write(Stream::BiDirectionalSeekableWriter&, const ArtFile& artFile);

	void VerifyImageIndexInBounds(std::size_t index);

private:
	// Read Functions
	static void ReadPalette(Stream::BiDirectionalSeekableReader& seekableReader, ArtFile& artFile);
	static void ReadImageMetadata(Stream::BiDirectionalSeekableReader& seekableReader, ArtFile& artFile);
	static void ReadAnimations(Stream::BiDirectionalSeekableReader& seekableReader, ArtFile& artFile);
	static Animation ReadAnimation(Stream::BiDirectionalSeekableReader& seekableReader);
	static Animation::Frame ReadFrame(Stream::BiDirectionalSeekableReader& seekableReader);
	static void VerifyCountsMatchHeader(const ArtFile& artFile, std::size_t frameCount, std::size_t layerCount, std::size_t unknownCount);


	// Write Functions
	static void WritePalettes(Stream::BiDirectionalSeekableWriter& seekableWriter, const ArtFile& artFile);
	static void WriteAnimations(Stream::BiDirectionalSeekableWriter& seekableWriter, const ArtFile& artFile);
	static void WriteAnimation(Stream::BiDirectionalSeekableWriter& seekableWriter, const Animation& animation);
	static void WriteFrame(Stream::BiDirectionalSeekableWriter& seekableWriter, const Animation::Frame& frame);


	void ValidateImageMetadata() const;
	void CountFrames(std::size_t& frameCount, std::size_t& layerCount, std::size_t& unknownCount) const;

	static const std::array<char, 4> TagPalette;
};
