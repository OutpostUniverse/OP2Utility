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
	class Writer;
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
	static ArtFile Read(Stream::Reader&& reader);
	void Write(std::string filename) const;
	void Write(Stream::Writer& writer) const;

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
	void WritePalettes(Stream::Writer& writer) const;
	void WriteAnimations(Stream::Writer& writer) const;
	static void WriteAnimation(Stream::Writer& writer, const Animation& animation);
	static void WriteFrame(Stream::Writer& writer, const Animation::Frame& frame);


	void ValidateImageMetadata() const;
	void CountFrames(std::size_t& frameCount, std::size_t& layerCount, std::size_t& unknownCount) const;

	static constexpr auto TagPalette = MakeTag("CPAL");
};
