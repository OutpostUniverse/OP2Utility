#pragma once

#include "PaletteHeader.h"
#include "ImageMeta.h"
#include "Animation.h"
#include "../Bitmap/Color.h"
#include "../Tag.h"
#include <vector>
#include <array>
#include <string>
#include <cstddef>

namespace OP2Utility
{
namespace Stream {
	class Reader;
	class Writer;
}

// ArtFile stores sprite metadata including color palettes. 
// Composing an in game sprite requires the metadata from ArtFile and the pixel values from OP2_ART.bmp.
// OP2_ART.bmp does not conform to standard bitmap file formatting.
// Artfile uses two color palettes for image storage, 1 and 8 bit indexed bitmaps
//  - All shadows sprites are repesented by 1 bit bitmaps
//  - All other sprites are represented by 8 bit bitmaps
struct ArtFile
{
public:
	// Palettes within ArtFile are stored in 8 bits of memory regardless of actual palette size (1 or 8 bit)
	std::vector<Palette8Bit> palettes;
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
}
