#include "TilesetHeaders.h"
#include "TilesetCommon.h"

namespace Tileset
{
	TilesetHeader TilesetHeader::Create(std::int32_t heightInTiles)
	{
		return TilesetHeader{
			SectionHeader(DefaultTagHead, DefaultSectionSize),
			DefaultTagCount,
			DefaultPixelWidth,
			heightInTiles * static_cast<int32_t>(DefaultPixelHeightMultiple),
			DefaultBitDepth,
			DefaultFlags
		};
	}

	void TilesetHeader::Validate() const
	{
		if (sectionHead.tag != DefaultTagHead) {
			throwReadError("Header Tag", sectionHead.tag, DefaultTagHead);
		}

		if (sectionHead.length != DefaultSectionSize) {
			throwReadError("Header Section Size", sectionHead.length, DefaultSectionSize);
		}

		if (pixelWidth != DefaultPixelWidth) {
			throwReadError("Pixel Width", pixelWidth, DefaultPixelWidth);
		}

		if (pixelHeight % DefaultPixelHeightMultiple != 0) {
			throw std::runtime_error("Tileset property Pixel Height reads " + std::to_string(pixelHeight) +
				". Pixel Height must be a multiple of " + std::to_string(DefaultPixelHeightMultiple) + ".");
		}

		if (tagCount != DefaultTagCount) {
			throwReadError("Header tag count", tagCount, DefaultTagCount);
		}
	}


	PpalHeader PpalHeader::Create()
	{
		return PpalHeader{
			SectionHeader(DefaultTagPpal, DefaultPpalSectionSize),
			SectionHeader(DefaultTagHead, DefaultHeadSectionSize),
			DefaultTagCount
		};
	}

	void PpalHeader::Validate() const
	{
		if (ppal.tag != DefaultTagPpal) {
			throwReadError("PPAL Tag", ppal.tag, DefaultTagPpal);
		}

		if (ppal.length != DefaultPpalSectionSize) {
			throwReadError("PPAL Section Size", ppal.length, DefaultPpalSectionSize);
		}

		if (head.tag != DefaultTagHead) {
			throwReadError("PPAL Head Tag", head.tag, DefaultTagHead);
		}

		if (head.length != DefaultHeadSectionSize) {
			throwReadError("PPAL Head Section Size", head.length, DefaultHeadSectionSize);
		}

		if (tagCount != DefaultTagCount) {
			throwReadError("PPAL Section Tag Count", tagCount, DefaultTagCount);
		}
	}
}
