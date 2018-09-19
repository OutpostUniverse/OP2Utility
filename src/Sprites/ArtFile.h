#pragma once

#include "ImageMeta.h"
#include "Animation.h"
#include "Color.h"
#include "BitCount.h"
#include <vector>
#include <array>
#include <string>
#include <cstddef>

namespace Stream {
	class SeekableReader;
	class SeekableWriter;
}

struct ArtFile
{
public:
	std::vector<Palette> palettes;
	std::vector<ImageMeta> imageMetas;
	std::vector<Animation> animations;
	uint32_t unknownAnimationCount;

	static ArtFile Read(std::string filename);
	static ArtFile Read(Stream::SeekableReader& seekableReader);
	static void Write(std::string filename, const ArtFile& artFile);
	static void Write(Stream::SeekableWriter&, const ArtFile& artFile);

	BitCount GetBitCount(std::size_t imageIndex);
	void CheckImageIndex(std::size_t index);

private:
	// Read Functions
	static void ReadPalette(Stream::SeekableReader& seekableReader, ArtFile& artFile);
	static void ReadImageMetadata(Stream::SeekableReader& seekableReader, ArtFile& artFile);
	static void ReadAnimations(Stream::SeekableReader& seekableReader, ArtFile& artFile);
	static Animation ReadAnimation(Stream::SeekableReader& seekableReader);
	static Animation::Frame ReadFrame(Stream::SeekableReader& seekableReader);
	static void VerifyCountsMatchHeader(const ArtFile& artFile, std::size_t frameCount, std::size_t layerCount, std::size_t unknownCount);


	// Write Functions
	static void WritePalettes(Stream::SeekableWriter& seekableWriter, const ArtFile& artFile);
	static void WriteAnimations(Stream::SeekableWriter& seekableWriter, const ArtFile& artFile);
	static void WriteAnimation(Stream::SeekableWriter& seekableWriter, const Animation& animation);
	static void WriteFrame(Stream::SeekableWriter& seekableWriter, const Animation::Frame& frame);


	void ValidateImageMetadata() const;
	void CountFrames(std::size_t& frameCount, std::size_t& layerCount, std::size_t& unknownCount) const;

	static const std::array<char, 4> TagPalette;
};