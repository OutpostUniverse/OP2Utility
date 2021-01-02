#pragma once

#include "TilesetLoader.h"
#include "SectionHeader.h"
#include "../Tag.h"
#include <cstdint>

namespace Tileset
{
	struct TilesetHeader
	{
		SectionHeader sectionHead;
		uint32_t tagCount;
		uint32_t pixelWidth;
		int32_t pixelHeight;
		uint32_t bitDepth;
		uint32_t flags;

		constexpr static auto DefaultTagHead = MakeTag("head");
		constexpr static uint32_t DefaultSectionSize = 0x14;
		constexpr static uint32_t DefaultTagCount = 2;
		constexpr static uint32_t DefaultPixelWidth = 32;
		constexpr static uint32_t DefaultPixelHeightMultiple = DefaultPixelWidth;
		constexpr static uint32_t DefaultBitDepth = 8;
		constexpr static uint32_t DefaultFlags = 8; // Unsure meaning of Flags

		static TilesetHeader Create(std::int32_t heightInTiles);

		void Validate() const;
	};
}
