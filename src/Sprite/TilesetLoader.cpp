#include "TilesetLoader.h"
#include "TilesetHeaders.h"
#include "TilesetCommon.h"
#include "../Bitmap/BitmapFile.h"
#include "../Stream/BidirectionalReader.h"
#include <cstdint>
#include <stdexcept>

namespace Tileset
{
	void ValidateFileSignatureHeader(const SectionHeader& fileSignatureHeader);
	void ValidatePaletteHeader(const SectionHeader& paletteHeader);
	void ValidatePixelHeader(const SectionHeader& pixelHeader, int32_t height);

	bool PeekIsCustomTileset(Stream::BidirectionalReader& reader)
	{
		Tag tag;
		reader.Read(tag);
		reader.SeekBackward(sizeof(tag));

		return tag == TagFileSignature;
	}

	BitmapFile ReadCustomTileset(Stream::Reader& reader)
	{
		SectionHeader fileSignature;
		reader.Read(fileSignature);
		ValidateFileSignatureHeader(fileSignature);

		TilesetHeader tilesetHeader;
		reader.Read(tilesetHeader);
		tilesetHeader.Validate();

		PpalHeader ppalHeader;
		reader.Read(ppalHeader);
		ppalHeader.Validate();

		SectionHeader paletteHeader;
		reader.Read(paletteHeader);
		ValidatePaletteHeader(paletteHeader);

		auto bitmapFile = BitmapFile::CreateDefaultIndexed(tilesetHeader.bitDepth, tilesetHeader.pixelWidth, tilesetHeader.pixelHeight);
		reader.Read(bitmapFile.palette);

		SectionHeader pixelHeader;
		reader.Read(pixelHeader);
		ValidatePixelHeader(pixelHeader, tilesetHeader.pixelHeight);

		reader.Read(bitmapFile.pixels);
		// Tilesets store red and blue Color values swapped from standard Bitmap file format
		bitmapFile.SwapRedAndBlue();

		ValidateTileset(bitmapFile);

		return bitmapFile;
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

	void ValidateFileSignatureHeader(const SectionHeader& fileSignatureHeader)
	{
		if (fileSignatureHeader.tag != TagFileSignature)
		{
			throwReadError("File Signature", fileSignatureHeader.tag, TagFileSignature);
		}

		if (fileSignatureHeader.length == 0)
		{
			throw std::runtime_error("Tileset property File Length reads " + 
				std::to_string(fileSignatureHeader.length) + ", which is too small.");
		}
	}

	void ValidatePaletteHeader(const SectionHeader& paletteHeader)
	{
		if (paletteHeader.tag != Tileset::DefaultTagData) {
			throwReadError("Palette Header Tag", paletteHeader.tag, Tileset::DefaultTagData);
		}

		if (paletteHeader.length != Tileset::DefaultPaletteHeaderSize) {
			throwReadError("Palette Header Section Size", paletteHeader.length, Tileset::DefaultPaletteHeaderSize);
		}
	}

	void ValidatePixelHeader(const SectionHeader& pixelHeader, int32_t height)
	{
		if (pixelHeader.tag != DefaultTagData) {
			throwReadError("Pixel Header Tag", pixelHeader.tag, DefaultTagData);
		}

		// Because tilesets are have a bitDepth of 8 and are always 32 pixels wide, 
		// can assume a padding of 0 bytes on each scan line.
		uint32_t expectedLenth = TilesetHeader::DefaultPixelWidth * height;
		if (pixelHeader.length != expectedLenth) {
			throwReadError("Pixel Header Length", pixelHeader.length, expectedLenth);
		}
	}
}
