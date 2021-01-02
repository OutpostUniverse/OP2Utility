#include "TilesetLoader.h"
#include "TilesetCommon.h"
#include "../Bitmap/BitmapFile.h"
#include "../Stream/BidirectionalReader.h"
#include <cstdint>
#include <stdexcept>

namespace Tileset
{
	bool PeekIsCustomTileset(Stream::BidirectionalReader& reader)
	{
		Tag tag;
		reader.Read(tag);
		reader.SeekBackward(sizeof(tag));

		return tag == TagFileSignature;
	}

	void ValidateTileset(const BitmapFile& tileset)
	{
		constexpr uint32_t DefaultPixelWidth = 32;
		constexpr uint32_t DefaultPixelHeightMultiple = DefaultPixelWidth;
		constexpr uint32_t DefaultBitDepth = 8;

		if (tileset.imageHeader.bitCount != DefaultBitDepth) {
			throwReadError("Bit Depth", tileset.imageHeader.bitCount, DefaultBitDepth);
		}

		if (tileset.imageHeader.width != DefaultPixelWidth) {
			throwReadError("Pixel Width", tileset.imageHeader.width, DefaultPixelWidth);
		}

		if (tileset.imageHeader.height % DefaultPixelHeightMultiple != 0) {
			throw std::runtime_error("Tileset property Pixel Height reads " + std::to_string(tileset.imageHeader.height) +
				". Expected value must be a multiple of " + std::to_string(DefaultPixelHeightMultiple) + " pixels");
		}
	}
}
