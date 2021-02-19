#pragma once

#include "ArtFile.h"
#include "../Stream/SliceReader.h"
#include <memory>
#include <string>
#include <cstddef>
#include <vector>

namespace OP2Utility
{
	// Loads Outpost 2 in game sprites from their custom format.
	// Sprites are reconditioned into standard indexed bitmap files after loading.
	// The sprites may be viewed and manipulated by standard bitmap applications and libraries after loading.
	// Outpost 2 tilesets (wells) are stored in a separate format from other in game sprites and cannot be loaded by this class.
	class SpriteLoader
	{
	public:
		SpriteLoader(std::string bmpFilename, std::shared_ptr<ArtFile> artFile);

		void ExtractImage(std::size_t index, const std::string& filenameOut);

		std::size_t ImageCount() const { return artFile->imageMetas.size(); }
		std::size_t AnimationCount() const { return artFile->animations.size(); }
		std::size_t FrameCount(std::size_t animationIndex) const;
		std::size_t LayerCount(std::size_t animationIndex, std::size_t frameIndex) const;

	private:
		// Bmp loader for Outpost 2 specific BMP file
		// Contains many images in pixels section with a default palette. 
		// Actual palette data and range of pixels to form each image is contained in the .prt file.
		Stream::FileReader bmpReader;
		std::shared_ptr<ArtFile> artFile;

		std::vector<Color> GetPalette(const ImageMeta& imageMeta);
		std::unique_ptr<Stream::FileSliceReader> GetPixels(std::size_t startingIndex, std::size_t length);
	};
}
