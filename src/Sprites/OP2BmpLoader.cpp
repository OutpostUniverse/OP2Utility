#include "OP2BmpLoader.h"
#include <stdexcept>

OP2BmpLoader::OP2BmpLoader(std::string bmpFilename, std::string artFilename) :
	bmpReader(bmpFilename), artFile(ArtFile::Read(artFilename)) { }

void OP2BmpLoader::ExtractImage(std::size_t index) 
{
	artFile.VerifyImageIndexInBounds(index);

	ImageMeta& imageMeta = artFile.imageMetas[index];
	auto pixels = GetPixels(imageMeta.pixelDataOffset, imageMeta.scanLineByteWidth * 8 * imageMeta.height);

	// TODO: Save pixels to a BMP file.
}

std::unique_ptr<Stream::FileSliceReader> OP2BmpLoader::GetPixels(std::size_t startingIndex, std::size_t length)
{
	return std::make_unique<Stream::FileSliceReader>(bmpReader.Slice(startingIndex, length));
}
