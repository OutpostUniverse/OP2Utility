#pragma once

#include "ArtFile.h"
#include "SectionHeader.h"
#include <array>
#include <cstdint>

namespace Stream {
	class SeekableReader;
}

namespace PaletteTag {
	extern const std::array<char, 4> Palette;
}

struct PaletteHeader {
	PaletteHeader(const ArtFile& artFile);
	PaletteHeader();

	SectionHeader overallHeader;
	SectionHeader sectionHeader;
	uint32_t remainingTagCount;
	SectionHeader dataHeader;

	void Validate();

private:
	static const std::array<char, 4> TagSection;
	static const std::array<char, 4> TagHeader;
	static const std::array<char, 4> TagData;
};
