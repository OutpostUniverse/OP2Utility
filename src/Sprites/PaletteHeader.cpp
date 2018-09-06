#include "PaletteHeader.h"

namespace PaletteTag {
	extern const std::array<char, 4> Palette{ 'C', 'P', 'A', 'L' };
	const std::array<char, 4> Section{ 'P', 'P', 'A', 'L' };
	const std::array<char, 4> Header{ 'h', 'e', 'a', 'd' };
	const std::array<char, 4> Data{ 'd', 'a', 't', 'a' };
}

PaletteHeader::PaletteHeader() : remainingTagCount(0) {}

PaletteHeader::PaletteHeader(const ArtFile& artFile)  : 
	overallHeader(SectionHeader(PaletteTag::Section, 
		sizeof(overallHeader) + 
		sizeof(sectionHeader) + 
		sizeof(remainingTagCount) + 
		static_cast<uint32_t>(artFile.palettes.size()) * sizeof(decltype(artFile.palettes)::value_type))),
	sectionHeader(SectionHeader(PaletteTag::Section, sizeof(remainingTagCount))),
	remainingTagCount(1),
	dataHeader(SectionHeader(PaletteTag::Data, static_cast<uint32_t>(artFile.palettes.size()) * sizeof(decltype(artFile.palettes)::value_type)))
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
	overallHeader.Validate(PaletteTag::Section);
	sectionHeader.Validate(PaletteTag::Header);
	dataHeader.Validate(PaletteTag::Data);

	if (overallHeader.length != sizeof(overallHeader) + sectionHeader.TotalLength() + sizeof(remainingTagCount) + dataHeader.TotalLength())
	{
		throw std::runtime_error("Lengths defined in palette headers do not match.");
	}
}
