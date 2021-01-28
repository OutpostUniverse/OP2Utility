#pragma once

#include "SectionHeader.h"
#include <array>
#include <cstdint>

namespace OP2Utility
{
	struct Color;

	using Palette8Bit = std::array<Color, 256>;

#pragma pack(push, 1) // Make sure structures are byte aligned

	struct PaletteHeader {
		PaletteHeader();
		static PaletteHeader CreatePaletteHeader();

		SectionHeader overallHeader;
		SectionHeader sectionHeader;
		uint32_t remainingTagCount;
		SectionHeader dataHeader;

		void Validate();
	};

	static_assert(4 + 3 * sizeof(SectionHeader) == sizeof(PaletteHeader), "PaletteHeader is an unexpected size");

#pragma pack(pop)
}
