#include "OP2BmpLoader.h"
#include "ImageHeader.h"
#include "IndexedBmpWriter.h"
#include <stdexcept>
#include <algorithm>

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

	IndexedBmpWriter::WritePitchIncluded(filenameOut, artFile.GetBitCount(index), imageMeta.width, -1 * imageMeta.height, palette, pixelContainer);
}

std::unique_ptr<Stream::FileSliceReader> OP2BmpLoader::GetPixels(std::size_t startingIndex, std::size_t length)
{
	return std::make_unique<Stream::FileSliceReader>(bmpReader.Slice(startingIndex, length));
}
