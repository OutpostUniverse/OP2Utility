#include "SpriteLoader.h"
#include "../Bitmap/ImageHeader.h"
#include "../Bitmap/BitmapFile.h"
#include <stdexcept>
#include <algorithm>
#include <limits>
#include <cstdint>

SpriteLoader::SpriteLoader(std::string bmpFilename, std::shared_ptr<ArtFile> artFile) :
	bmpReader(bmpFilename), artFile(artFile) { }

void SpriteLoader::ExtractImage(std::size_t index, const std::string& filenameOut) 
{
	artFile->VerifyImageIndexInBounds(index);

	ImageMeta& imageMeta = artFile->imageMetas[index];

	std::vector<Color> palette = GetPalette(imageMeta);

	// Palette length is always 256 for OP2's master BMP
	const std::size_t op2PaletteLength = 256;
	std::size_t pixelOffset = static_cast<std::size_t>(imageMeta.pixelDataOffset) + 14 + sizeof(ImageHeader) + op2PaletteLength * sizeof(Color);

	auto pixels = GetPixels(pixelOffset, static_cast<std::size_t>(imageMeta.scanLineByteWidth) * imageMeta.height);

	std::vector<uint8_t> pixelContainer(static_cast<std::size_t>(imageMeta.scanLineByteWidth) * imageMeta.height);
	(*pixels).Read(pixelContainer);

	// Outpost 2 stores pixels in normal raster scan order (top-down). This requires a negative height for BMP file format.
	if (imageMeta.height > INT32_MAX) {
		throw std::runtime_error("Image height is too large to fit in standard bitmap file format.");
	}

	BitmapFile::WriteIndexed(filenameOut, imageMeta.GetBitCount(), imageMeta.width, -static_cast<int32_t>(imageMeta.height), palette, pixelContainer);
}

std::vector<Color> SpriteLoader::GetPalette(const ImageMeta& imageMeta)
{
	const uint16_t bitCount = imageMeta.type.isShadow ? 1 : 8;

	std::vector<Color> palette(std::size_t(1) << bitCount);

	std::copy(artFile->palettes[imageMeta.paletteIndex].begin(), artFile->palettes[imageMeta.paletteIndex].begin() + palette.size(), palette.begin());

	return palette;
}

std::unique_ptr<Stream::FileSliceReader> SpriteLoader::GetPixels(std::size_t startingIndex, std::size_t length)
{
	return std::make_unique<Stream::FileSliceReader>(bmpReader.Slice(startingIndex, length));
}

std::size_t SpriteLoader::FrameCount(std::size_t animationIndex) const {
	return artFile->animations[animationIndex].frames.size();
}

std::size_t SpriteLoader::LayerCount(std::size_t animationIndex, std::size_t frameIndex) const {
	return artFile->animations[animationIndex].frames[frameIndex].layers.size();
}
