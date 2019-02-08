#pragma once

#include "ArtFile.h"
#include "SectionHeader.h"
#include <array>
#include <cstdint>

namespace Stream {
	class BidirectionalSeekableReader;
}

#pragma pack(push, 1) // Make sure structures are byte aligned

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

static_assert(4 + 3 * sizeof(SectionHeader) == sizeof(PaletteHeader), "PaletteHeader is an unexpected size");

#pragma pack(pop)