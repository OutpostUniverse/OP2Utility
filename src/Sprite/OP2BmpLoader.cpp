#include "OP2BmpLoader.h"
#include "../Bitmap/ImageHeader.h"
#include "../Bitmap/BitmapFile.h"
#include <stdexcept>
#include <algorithm>
#include <limits>

OP2BmpLoader::OP2BmpLoader(std::string bmpFilename, std::string artFilename) :
	bmpReader(bmpFilename), artFile(ArtFile::Read(artFilename)) { }

void OP2BmpLoader::ExtractImage(std::size_t index, const std::string& filenameOut) 
{
	artFile.VerifyImageIndexInBounds(index);

	ImageMeta& imageMeta = artFile.imageMetas[index];

	std::vector<Color> palette = GetPalette(imageMeta);

	// Palette length is always 256 for OP2's master BMP
	const std::size_t op2PaletteLength = 256;
	std::size_t pixelOffset = static_cast<std::size_t>(imageMeta.pixelDataOffset) + 14 + sizeof(ImageHeader) + op2PaletteLength * sizeof(Color);

	auto pixels = GetPixels(pixelOffset, imageMeta.scanLineByteWidth * imageMeta.height);

	std::vector<uint8_t> pixelContainer(imageMeta.scanLineByteWidth * imageMeta.height);
	(*pixels).Read(pixelContainer);

	// Outpost 2 stores pixels in normal raster scan order (top-down). This requires a negative height for BMP file format.
	if (imageMeta.height > INT32_MAX) {
		throw std::runtime_error("Image height is too large to fit in standard bitmap file format.");
	}

	BitmapFile::WriteIndexed(filenameOut, imageMeta.GetBitCount(), imageMeta.width, -static_cast<int32_t>(imageMeta.height), palette, pixelContainer);
}

std::vector<Color> OP2BmpLoader::GetPalette(const ImageMeta& imageMeta)
{
	std::vector<Color> palette;

	if (imageMeta.IsShadow())
	{
		// Shadow graphic uses a 2 color palette
		palette.resize(2);
	}
	else
	{
		palette.resize(artFile.palettes[imageMeta.paletteIndex].size());
	}

	std::copy(artFile.palettes[imageMeta.paletteIndex].begin(), artFile.palettes[imageMeta.paletteIndex].end(), palette.begin());

	return palette;
}

std::unique_ptr<Stream::FileSliceReader> OP2BmpLoader::GetPixels(std::size_t startingIndex, std::size_t length)
{
	return std::make_unique<Stream::FileSliceReader>(bmpReader.Slice(startingIndex, length));
}
