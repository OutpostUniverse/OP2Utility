#include "OP2BmpLoader.h"
#include "../Bitmap/ImageHeader.h"
#include "../Bitmap/BitmapFile.h"
#include <stdexcept>
#include <algorithm>
#include <limits>

const std::vector<Color> DefaultMonochromePalette{ Color{0, 0, 0}, Color{255, 255, 255} };

OP2BmpLoader::OP2BmpLoader(std::string bmpFilename, std::string artFilename) :
	bmpReader(bmpFilename), artFile(ArtFile::Read(artFilename)) { }

OP2BmpLoader::OP2BmpLoader(std::string bmpFilename, Stream::BidirectionalSeekableReader& artFileStream) :
	bmpReader(bmpFilename), artFile(ArtFile::Read(artFileStream)) { }

void OP2BmpLoader::ExtractImage(std::size_t index, const std::string& filenameOut) 
{
	artFile.VerifyImageIndexInBounds(index);

	const ImageMeta& imageMeta = artFile.imageMetas[index];

	const auto palette = CreatePalette(imageMeta);

	const std::size_t pixelOffset = imageMeta.pixelDataOffset + 14 + sizeof(ImageHeader) + palette.size() * sizeof(Color);

	const auto pixels = GetPixels(pixelOffset, imageMeta.scanLineByteWidth * imageMeta.height);

	std::vector<uint8_t> pixelContainer(imageMeta.scanLineByteWidth * imageMeta.height);
	(*pixels).Read(pixelContainer);

	// Outpost 2 stores pixels in normal raster scan order (top-down). This requires a negative height for BMP file format.
	if (imageMeta.height > INT32_MAX) {
		throw std::runtime_error("Image height is too large to fit in standard bitmap file format.");
	}

	BitmapFile::WriteIndexed(filenameOut, imageMeta.GetBitCount(), imageMeta.width, -static_cast<int32_t>(imageMeta.height), imageMeta.scanLineByteWidth, palette, pixelContainer);
}

std::size_t OP2BmpLoader::FrameCount(std::size_t animationIndex) const {
	return artFile.animations[animationIndex].frames.size();
}

std::size_t OP2BmpLoader::LayerCount(std::size_t animationIndex, std::size_t frameIndex) const {
	return artFile.animations[animationIndex].frames[frameIndex].layers.size();
}

std::vector<Color> OP2BmpLoader::CreatePalette(const ImageMeta& imageMeta) const
{
	std::vector<Color> palette;

	if (imageMeta.GetBitCount() == 1) {
		return DefaultMonochromePalette;
	}
	else {
		palette.resize(artFile.palettes[imageMeta.paletteIndex].size());
		std::copy(artFile.palettes[imageMeta.paletteIndex].begin(), artFile.palettes[imageMeta.paletteIndex].end(), palette.begin());
	}

	return palette;
}

std::unique_ptr<Stream::FileSliceReader> OP2BmpLoader::GetPixels(std::size_t startingIndex, std::size_t length)
{
	return std::make_unique<Stream::FileSliceReader>(bmpReader.Slice(startingIndex, length));
}
