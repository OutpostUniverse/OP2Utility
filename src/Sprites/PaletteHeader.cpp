#include "PaletteHeader.h"

const std::array<char, 4> PaletteHeader::TagSection{ 'P', 'P', 'A', 'L' };
const std::array<char, 4> PaletteHeader::TagHeader{ 'h', 'e', 'a', 'd' };
const std::array<char, 4> PaletteHeader::TagData{ 'd', 'a', 't', 'a' };

PaletteHeader::PaletteHeader() : remainingTagCount(0) {}

PaletteHeader::PaletteHeader(const ArtFile& artFile)  : 
	overallHeader(SectionHeader(TagSection, 
		sizeof(overallHeader) + 
		sizeof(sectionHeader) + 
		sizeof(remainingTagCount) + 
		static_cast<uint32_t>(artFile.palettes.size()) * sizeof(decltype(artFile.palettes)::value_type))),
	sectionHeader(SectionHeader(TagHeader, sizeof(remainingTagCount))),
	remainingTagCount(1),
	dataHeader(SectionHeader(TagData, static_cast<uint32_t>(artFile.palettes.size()) * sizeof(decltype(artFile.palettes)::value_type)))
{ 
	uint64_t paletteSectionSize =
		sizeof(overallHeader) +
		sizeof(sectionHeader) +
		sizeof(remainingTagCount) +
		artFile.palettes.size() * sizeof(decltype(artFile.palettes)::value_type);

	if (paletteSectionSize > UINT32_MAX) {
		throw std::runtime_error("PRT palettes section is too large.");
	}
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
