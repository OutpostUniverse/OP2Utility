#include "TilesetLoader.h"
#include "../Bitmap/BitmapFile.h"
#include "../Stream/BidirectionalReader.h"
#include <cstdint>
#include <stdexcept>

namespace Tileset
{
	void throwReadError(std::string propertyName, std::string value, std::string expectedValue);
	void throwReadError(std::string propertyName, uint32_t value, uint32_t expectedValue);

	std::string formatReadErrorMessage(std::string propertyName, std::string value, std::string expectedValue);

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

	void throwReadError(std::string propertyName, std::string value, std::string expectedValue)
	{
		throw std::runtime_error(formatReadErrorMessage(propertyName, value, expectedValue));
	}

	void throwReadError(std::string propertyName, uint32_t value, uint32_t expectedValue)
	{
		throwReadError(propertyName, std::to_string(value), std::to_string(expectedValue));
	}

	std::string formatReadErrorMessage(std::string propertyName, std::string value, std::string expectedValue)
	{
		return "Tileset property " + propertyName + " reads. Expected a value of " + expectedValue + ".";
	}
}
