#include "PaletteHeader.h"
#include "../Bitmap/Color.h"


constexpr auto TagSection = MakeTag("PPAL");
constexpr auto TagHeader = MakeTag("head");
constexpr auto TagData = MakeTag("data");


PaletteHeader::PaletteHeader() : remainingTagCount(0) {}

PaletteHeader PaletteHeader::CreatePaletteHeader()
{
	PaletteHeader paletteHeader;
	paletteHeader.remainingTagCount = 1;

	const uint64_t dataSize = sizeof(Palette);

	uint64_t overallSize = 4 + sizeof(PaletteHeader::overallHeader) + 
		sizeof(PaletteHeader::sectionHeader) + sizeof(PaletteHeader::remainingTagCount) + dataSize;

	if (overallSize > UINT32_MAX) {
		throw std::runtime_error("PRT palettes section is too large.");
	}

	paletteHeader.overallHeader = SectionHeader(TagSection, static_cast<uint32_t>(overallSize));
	paletteHeader.sectionHeader = SectionHeader(SectionHeader(TagHeader, sizeof(remainingTagCount)));
	paletteHeader.dataHeader = SectionHeader(TagData, static_cast<uint32_t>(dataSize));

	return paletteHeader;
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
