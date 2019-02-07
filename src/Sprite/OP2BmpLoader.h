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

	void ExtractImage(std::size_t index, const std::string& filenameOut);

private:
	// Bmp loader for Outpost 2 specific BMP file
	// Contains many images in pixels section with a default palette. 
	// Actual palette data and range of pixels to form each image is contained in the .prt file.
	Stream::FileReader bmpReader;
	ArtFile artFile;

	std::unique_ptr<Stream::FileSliceReader> GetPixels(std::size_t startingIndex, std::size_t length);
};
