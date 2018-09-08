#include "ArtWriter.h"
#include "PaletteHeader.h"
#include "ArtFile.h"
#include "../Streams/FileWriter.h"
#include <cstdint>
#include <stdexcept>

namespace ArtWriter
{
	// Anonymous namespace to hold private methods
	//namespace {
	void WritePalettes(Stream::SeekableWriter& seekableWriter, const ArtFile& artFile);
	void WriteAnimations(Stream::SeekableWriter& seekableWriter, const ArtFile& artFile);
	void WriteAnimation(Stream::SeekableWriter& seekableWriter, const Animation& animation);
	//}


	// ==== Public methohds ====


	void Write(std::string filename, const ArtFile& artFile)
	{
		Stream::FileWriter artWriter(filename);
		Write(artWriter, artFile);
	}

	void Write(Stream::SeekableWriter& seekableWriter, const ArtFile& artFile)
	{
		artFile.ValidateImageMetadata();

		WritePalettes(seekableWriter, artFile);

		seekableWriter.Write(artFile.imageMetas);

		WriteAnimations(seekableWriter, artFile);
	}


	// ==== Public methohds ====


	void WritePalettes(Stream::SeekableWriter& seekableWriter, const ArtFile& artFile)
	{
		if (artFile.palettes.size() > UINT32_MAX) {
			throw std::runtime_error("Art file contains too many palettes.");
		}

		seekableWriter.Write(SectionHeader(PaletteTag::Palette, static_cast<uint32_t>(artFile.palettes.size())));
		seekableWriter.Write(PaletteHeader(artFile));
	}

	void WriteAnimations(Stream::SeekableWriter& seekableWriter, const ArtFile& artFile)
	{
		if (artFile.animations.size() > UINT32_MAX) {
			throw std::runtime_error("There are too many animations contained in the ArtFile.");
		}

		seekableWriter.Write(static_cast<uint32_t>(artFile.animations.size()));
		
		std::size_t frameCount;
		std::size_t subframeCount;
		std::size_t unknownCount;
		artFile.CountFrames(frameCount, subframeCount, unknownCount);

		if (frameCount > UINT32_MAX) {
			throw std::runtime_error("There are too many frames to write to file.");
		}

		if (subframeCount > UINT32_MAX) {
			throw std::runtime_error("There are too many subframes to write to file.");
		}

		if (unknownCount > UINT32_MAX) {
			throw std::runtime_error("There are too many unknown container items to write to file.");
		}

		seekableWriter.Write(static_cast<uint32_t>(frameCount));
		seekableWriter.Write(static_cast<uint32_t>(subframeCount));
		seekableWriter.Write(static_cast<uint32_t>(unknownCount));

		for (const auto& animation : artFile.animations)
		{
			WriteAnimation(seekableWriter, animation);
		}
	}

	void WriteAnimation(Stream::SeekableWriter& seekableWriter, const Animation& animation)
	{

	}
}
