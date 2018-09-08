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
	Animation::Frame ReadFrame(Stream::SeekableReader& seekableReader);
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
		seekableReader.Read<uint32_t>(artFile.imageMetas);

		artFile.ValidateImageMetadata();
	}

	void ReadAnimations(Stream::SeekableReader& seekableReader, ArtFile& artFile)
	{
		uint32_t animationCount;
		seekableReader.Read(animationCount);
		artFile.animations.resize(animationCount);

		uint32_t frameCount;
		seekableReader.Read(frameCount);

		uint32_t subFrameCount;
		seekableReader.Read(subFrameCount);

		uint32_t frameOptionalCount;
		seekableReader.Read(frameOptionalCount);

		for (uint32_t i = 0; i < animationCount; ++i)
		{
			artFile.animations[i] = ReadAnimation(seekableReader);
		}

		std::size_t actualFrameCount = 0;
		std::size_t actualSubFrameCount = 0;
		for (Animation animation : artFile.animations) {
			actualFrameCount += animation.frames.size();

			for (Animation::Frame frame : animation.frames) {
				actualSubFrameCount += frame.subFrames.size();
			}
		}

		if (actualFrameCount != frameCount) {
			throw std::runtime_error("Frame count does not match");
		}

		if (actualSubFrameCount != subFrameCount) {
			throw std::runtime_error("Sub-frame count does not match.");
		}
	}

	Animation ReadAnimation(Stream::SeekableReader& seekableReader)
	{
		Animation animationMeta;

		seekableReader.Read(animationMeta.unknown);
		seekableReader.Read(animationMeta.selectionRect);
		seekableReader.Read(animationMeta.pixelDisplacement);
		seekableReader.Read(animationMeta.unknown2);

		uint32_t frameCount;
		seekableReader.Read(frameCount);
		animationMeta.frames.resize(frameCount);

		for (uint32_t i = 0; i < frameCount; ++i) {
			animationMeta.frames[i] = ReadFrame(seekableReader);
		}

		uint32_t unknownContainerCount;
		seekableReader.Read(unknownContainerCount);

		animationMeta.unknownContainer.resize(unknownContainerCount);
		seekableReader.Read(animationMeta.unknownContainer);

		return animationMeta;
	}

	Animation::Frame ReadFrame(Stream::SeekableReader& seekableReader) {
		Animation::Frame frame;

		uint8_t subframeCount;
		seekableReader.Read(subframeCount);
		seekableReader.Read(frame.unknown);

		if (subframeCount & 0x80) {
			seekableReader.Read(frame.optional1);
			seekableReader.Read(frame.optional2);
		}
		if (frame.unknown & 0x80) {
			seekableReader.Read(frame.optional3);
			seekableReader.Read(frame.optional4);
		}

		frame.subFrames.resize(subframeCount & 0x7F);
		for (int i = 0; i < (subframeCount & 0x7F); ++i) {
			seekableReader.Read(frame.subFrames[i]);
		}

		return frame;
	}
}
