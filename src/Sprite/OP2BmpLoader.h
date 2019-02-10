#pragma once

#include "ArtFile.h"
#include "../Stream/SliceReader.h"
#include <memory>
#include <string>
#include <cstddef>

class OP2BmpLoader
{
public:
	OP2BmpLoader(std::string bmpFilename, std::string artFilename);
	OP2BmpLoader(std::string bmpFilename, Stream::BidirectionalSeekableReader& artFileStream);

	void ExtractImage(std::size_t index, const std::string& filenameOut);
	
	inline std::size_t ImageCount() const { return artFile.imageMetas.size(); }
	inline std::size_t AnimationCount() const { return artFile.animations.size(); }
	std::size_t FrameCount(std::size_t animationIndex) const;
	std::size_t LayerCount(std::size_t animationIndex, std::size_t frameIndex) const;

private:
	// Bmp loader for Outpost 2 specific BMP file
	// Contains many images in pixels section with a default palette. 
	// Actual palette data and range of pixels to form each image is contained in the .prt file.
	Stream::FileReader bmpReader;
	ArtFile artFile;

	std::vector<Color> CreatePalette(const ImageMeta& imageMeta) const;
	std::unique_ptr<Stream::FileSliceReader> GetPixels(std::size_t startingIndex, std::size_t length);
};
