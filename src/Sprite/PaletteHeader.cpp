#include "PaletteHeader.h"


constexpr auto TagSection = MakeTag("PPAL");
constexpr auto TagHeader = MakeTag("head");
constexpr auto TagData = MakeTag("data");


PaletteHeader::PaletteHeader() : remainingTagCount(0) {}

PaletteHeader::PaletteHeader(const ArtFile& artFile)  : remainingTagCount(1)
{
	uint64_t dataSize = sizeof(Palette8Bit);

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
