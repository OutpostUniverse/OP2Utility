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

	std::vector<Color> palette(artFile.palettes[imageMeta.paletteIndex].size());
	std::copy(artFile.palettes[imageMeta.paletteIndex].begin(), artFile.palettes[imageMeta.paletteIndex].end(), palette.begin());

	std::size_t pixelOffset = imageMeta.pixelDataOffset + 14 + sizeof(ImageHeader) + palette.size() * sizeof(Color);

	auto pixels = GetPixels(pixelOffset, imageMeta.scanLineByteWidth * imageMeta.height);

	std::vector<uint8_t> pixelContainer(imageMeta.scanLineByteWidth * imageMeta.height);
	(*pixels).Read(pixelContainer);

	// Outpost 2 stores pixels in normal raster scan order (top-down). This requires a negative height for BMP file format.
	if (imageMeta.height > INT32_MAX) {
		throw std::runtime_error("Image height is too large to fit in standard bitmap file format.");
	}

	BitmapFile::WriteIndexed(filenameOut, imageMeta.GetBitCount(), imageMeta.width, -static_cast<int32_t>(imageMeta.height), palette, pixelContainer);
}

std::unique_ptr<Stream::FileSliceReader> OP2BmpLoader::GetPixels(std::size_t startingIndex, std::size_t length)
{
	return std::make_unique<Stream::FileSliceReader>(bmpReader.Slice(startingIndex, length));
}
