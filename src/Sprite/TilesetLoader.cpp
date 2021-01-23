#include "TilesetLoader.h"
#include "TilesetHeaders.h"
#include "TilesetCommon.h"
#include "../Bitmap/BitmapFile.h"
#include "../Stream/BidirectionalReader.h"
#include "../Stream/Writer.h"
#include <cstdint>
#include <stdexcept>

namespace Tileset
{
	void ValidateFileSignatureHeader(const SectionHeader& fileSignatureHeader);
	void ValidatePaletteHeader(const SectionHeader& paletteHeader);
	void ValidatePixelHeader(const SectionHeader& pixelHeader, uint32_t height);
	uint32_t CalculatePbmpSectionSize(uint32_t pixelLength);
	uint32_t CalculatePixelHeaderLength(uint32_t height);
	void SwapPaletteRedAndBlue(std::vector<Color>& palette);


	bool PeekIsCustomTileset(Stream::BidirectionalReader&& reader)
	{
		return PeekIsCustomTileset(reader); // Delegate to lvalue overload
	}

	bool PeekIsCustomTileset(Stream::BidirectionalReader& reader)
	{
		Tag tag;
		reader.Read(tag);
		reader.SeekBackward(sizeof(tag));

		return tag == TagFileSignature;
	}

	BitmapFile ReadTileset(Stream::BidirectionalReader&& reader)
	{
		return ReadTileset(reader); // Delegate to lvalue overload
	}

	BitmapFile ReadTileset(Stream::BidirectionalReader& reader)
	{
		if (PeekIsCustomTileset(reader)) {
			return ReadCustomTileset(reader);
		}
		
		try {
			auto tileset = BitmapFile::ReadIndexed(reader);
			ValidateTileset(tileset);
			return tileset;
		}
		catch (std::exception& e) {
			throw std::runtime_error("Unable to read tileset represented as standard bitmap. " + std::string(e.what()));
		}
	}

	BitmapFile ReadCustomTileset(Stream::Reader&& reader)
	{
		return ReadCustomTileset(reader); // Delegate to lvalue overload
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

		auto bitmapFile = BitmapFile::CreateIndexed(tilesetHeader.bitDepth, tilesetHeader.pixelWidth, tilesetHeader.pixelHeight * -1);
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

	void WriteCustomTileset(Stream::Writer& writer, BitmapFile tileset)
	{
		ValidateTileset(tileset);

		// OP2 Custom Tileset assumes a positive height and TopDown Scan Line (Contradicts Windows Bitmap File Format)
		if (tileset.ScanLineOrientation() == ScanLineOrientation::BottomUp) {
			tileset.InvertScanLines();
		}
		auto absoluteHeight = tileset.AbsoluteHeight();

		SectionHeader fileSignature{ TagFileSignature, CalculatePbmpSectionSize(absoluteHeight) };
		TilesetHeader tilesetHeader = TilesetHeader::Create(absoluteHeight / TilesetHeader::DefaultPixelHeightMultiple);
		PpalHeader ppalHeader = PpalHeader::Create();
		
		SectionHeader paletteHeader{ DefaultTagData, DefaultPaletteHeaderSize };
		SwapPaletteRedAndBlue(tileset.palette);

		SectionHeader pixelHeader{ DefaultTagData, CalculatePixelHeaderLength(absoluteHeight) };

		writer.Write(fileSignature);
		writer.Write(tilesetHeader);
		writer.Write(ppalHeader);
		writer.Write(paletteHeader);
		writer.Write(tileset.palette);
		writer.Write(pixelHeader);
		writer.Write(tileset.pixels);
	}

	void ValidateTileset(const BitmapFile& tileset)
	{
		constexpr int32_t DefaultPixelWidth = 32;
		constexpr uint32_t DefaultPixelHeightMultiple = DefaultPixelWidth;
		constexpr uint16_t DefaultBitCount = 8;

		if (tileset.imageHeader.bitCount != DefaultBitCount) {
			throwReadError("Bit Depth", tileset.imageHeader.bitCount, DefaultBitCount);
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

	void ValidatePixelHeader(const SectionHeader& pixelHeader, uint32_t height)
	{
		if (pixelHeader.tag != DefaultTagData) {
			throwReadError("Pixel Header Tag", pixelHeader.tag, DefaultTagData);
		}

		auto expectedLenth = CalculatePixelHeaderLength(height);
		if (pixelHeader.length != expectedLenth) {
			throwReadError("Pixel Header Length", pixelHeader.length, expectedLenth);
		}
	}

	uint32_t CalculatePbmpSectionSize(uint32_t pixelLength)
	{
		return sizeof(Tag) + // PBMP Header
			sizeof(TilesetHeader) +
			sizeof(PpalHeader) +
			sizeof(Tag) + DefaultPaletteHeaderSize + //Palette and Header
			sizeof(Tag) + CalculatePixelHeaderLength(pixelLength) - //Pixels and Header
			16;
	}

	uint32_t CalculatePixelHeaderLength(uint32_t height)
	{
		// Because tilesets are have a bitDepth of 8 and are always 32 pixels wide, 
		// can assume a padding of 0 bytes on each scan line.
		return TilesetHeader::DefaultPixelWidth * height;
	}

	void SwapPaletteRedAndBlue(std::vector<Color>& palette)
	{
		for (auto& color : palette) {
			color.SwapRedAndBlue();
		}
	}
}
