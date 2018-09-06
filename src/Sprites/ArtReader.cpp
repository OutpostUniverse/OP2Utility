#include "ArtReader.h"
#include "ArtFile.h"
#include "PaletteHeader.h"
#include "../Streams/FileReader.h"
#include <stdexcept>
#include <vector>

namespace ArtReader {
	// Anonymous namespace to hold private methods
	//namespace {
	void ReadPalette(Stream::SeekableReader& seekableReader, ArtFile& artFile);
	void ReadImageMetadata(Stream::SeekableReader& seekableReader, ArtFile& artFile);
	void ReadAnimations(Stream::SeekableReader& seekableReader, ArtFile& artFile);
	Animation ReadAnimation(Stream::SeekableReader& seekableReader);
	Frame ReadFrame(Stream::SeekableReader& seekableReader);
	//}


	// ==== Public methohds ====


	ArtFile Read(std::string filename) {
		Stream::FileReader mapReader(filename);
		return Read(mapReader);
	}

	ArtFile Read(Stream::SeekableReader& seekableReader) {
		ArtFile artFile;

		ReadPalette(seekableReader, artFile);

		ReadImageMetadata(seekableReader, artFile);

		ReadAnimations(seekableReader, artFile);

		return artFile;
	}


	// == Private methods ==


	void ReadPalette(Stream::SeekableReader& seekableReader, ArtFile& artFile)
	{
		SectionHeader paletteHeader;
		seekableReader.Read(paletteHeader);
		paletteHeader.Validate(PaletteTag::Palette);

		artFile.palettes.resize(paletteHeader.length);

		for (uint32_t i = 0; i < paletteHeader.length; ++i) {
			PaletteHeader paletteHeader;
			seekableReader.Read(paletteHeader);

			paletteHeader.Validate();

			seekableReader.Read(artFile.palettes[i]);
		}
	}

	void ReadImageMetadata(Stream::SeekableReader& seekableReader, ArtFile& artFile)
	{
		uint32_t imageCount;
		seekableReader.Read(imageCount);

		artFile.imageMetas.resize(imageCount);
		seekableReader.Read(artFile.imageMetas);

		artFile.ValidateImageMetadata();
	}

	void ReadAnimations(Stream::SeekableReader& seekableReader, ArtFile& artFile)
	{
		uint32_t animationCount;
		seekableReader.Read(animationCount);
		artFile.animations.resize(animationCount);

		uint32_t frameDataCount;
		seekableReader.Read(frameDataCount);

		uint32_t frameComponentCount;
		seekableReader.Read(frameComponentCount);

		uint32_t frameOptionalCount;
		seekableReader.Read(frameOptionalCount);

		for (uint32_t i = 0; i < animationCount; ++i)
		{
			artFile.animations[i] = ReadAnimation(seekableReader);
		}
	}

	Animation ReadAnimation(Stream::SeekableReader& seekableReader)
	{
		Animation animationMeta;

		seekableReader.Read(animationMeta.unknown);
		seekableReader.Read(animationMeta.selectionRect);
		seekableReader.Read(animationMeta.pixelXDisplacement);
		seekableReader.Read(animationMeta.pixelYDisplacement);
		seekableReader.Read(animationMeta.unknown2);

		uint32_t frameCount;
		seekableReader.Read(frameCount);
		animationMeta.frames.resize(frameCount);

		for (uint32_t i = 0; i < frameCount; ++i) {
			animationMeta.frames[i] = ReadFrame(seekableReader);
		}

		seekableReader.Read(animationMeta.unknownArtContainerCount);

		animationMeta.unknownArtContainer.resize(animationMeta.unknownArtContainerCount);
		seekableReader.Read(animationMeta.unknownArtContainer);

		return animationMeta;
	}

	Frame ReadFrame(Stream::SeekableReader& seekableReader) {
		Frame frame;
		seekableReader.Read(frame.subframes);
		seekableReader.Read(frame.unknown);

		if (frame.subframes & 0x80) {
			frame.subframes = frame.subframes & 0x7F;
			seekableReader.Read(frame.optional1);
			seekableReader.Read(frame.optional2);
		}
		if (frame.unknown & 0x80) {
			frame.unknown = frame.unknown & 0x7F;
			seekableReader.Read(frame.optional3);
			seekableReader.Read(frame.optional4);
		}

		frame.pictures.resize(frame.subframes);
		seekableReader.Read(frame.subframes);

		return frame;
	}
}
