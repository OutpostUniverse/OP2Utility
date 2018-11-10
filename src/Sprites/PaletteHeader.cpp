#include "PaletteHeader.h"

const std::array<char, 4> PaletteHeader::TagSection{ 'P', 'P', 'A', 'L' };
const std::array<char, 4> PaletteHeader::TagHeader{ 'h', 'e', 'a', 'd' };
const std::array<char, 4> PaletteHeader::TagData{ 'd', 'a', 't', 'a' };

PaletteHeader::PaletteHeader() : remainingTagCount(0) {}

PaletteHeader::PaletteHeader(const ArtFile& artFile)  : remainingTagCount(1)
{
	uint64_t dataSize = sizeof(Palette);

	uint64_t overallSize = 4 + sizeof(overallHeader) + sizeof(sectionHeader) + sizeof(remainingTagCount) + dataSize;

	if (overallSize > UINT32_MAX) {
		throw std::runtime_error("PRT palettes section is too large.");
	}

	overallHeader = SectionHeader(TagSection, static_cast<uint32_t>(overallSize));
	sectionHeader = SectionHeader(SectionHeader(TagHeader, sizeof(remainingTagCount)));
	dataHeader = SectionHeader(TagData, static_cast<uint32_t>(dataSize));
}


void PaletteHeader::Validate()
{
	overallHeader.Validate(TagSection);
	sectionHeader.Validate(TagHeader);
	dataHeader.Validate(TagData);

	if (overallHeader.length != sizeof(overallHeader) + sectionHeader.TotalLength() + sizeof(remainingTagCount) + dataHeader.TotalLength())
	{
		throw std::runtime_error("Lengths defined in palette headers do not match.");
	}
}
