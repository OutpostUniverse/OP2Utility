#include "ArtFile.h"
#include "PaletteHeader.h"
#include "../Streams/FileReader.h"
#include <stdexcept>
#include <cstdint>
#include <cstddef>

ArtFile ArtFile::Read(std::string filename) {
	Stream::FileReader mapReader(filename);
	return Read(mapReader);
}

ArtFile ArtFile::Read(Stream::SeekableReader& seekableReader) {
	ArtFile artFile;

	ReadPalette(seekableReader, artFile);
	ReadImageMetadata(seekableReader, artFile);
	ReadAnimations(seekableReader, artFile);

	return artFile;
}

void ArtFile::ReadPalette(Stream::SeekableReader& seekableReader, ArtFile& artFile)
{
	SectionHeader paletteSectionHeader;
	seekableReader.Read(paletteSectionHeader);
	paletteSectionHeader.Validate(TagPalette);

	artFile.palettes.resize(paletteSectionHeader.length);

	for (uint32_t i = 0; i < paletteSectionHeader.length; ++i) {
		PaletteHeader paletteHeader;
		seekableReader.Read(paletteHeader);

		paletteHeader.Validate();

		seekableReader.Read(artFile.palettes[i]);
	}
}

void ArtFile::ReadImageMetadata(Stream::SeekableReader& seekableReader, ArtFile& artFile)
{
	seekableReader.Read<uint32_t>(artFile.imageMetas);

	artFile.ValidateImageMetadata();
}

void ArtFile::ReadAnimations(Stream::SeekableReader& seekableReader, ArtFile& artFile)
{
	uint32_t animationCount;
	seekableReader.Read(animationCount);
	artFile.animations.resize(animationCount);

	uint32_t frameCount;
	seekableReader.Read(frameCount);

	uint32_t layerCount;
	seekableReader.Read(layerCount);

	uint32_t unknownCount;
	seekableReader.Read(unknownCount);

	for (uint32_t i = 0; i < animationCount; ++i)
	{
		artFile.animations[i] = ReadAnimation(seekableReader);
	}

	VerifyFrameCount(artFile, frameCount, layerCount, unknownCount);
}

Animation ArtFile::ReadAnimation(Stream::SeekableReader& seekableReader)
{
	Animation animation;

	seekableReader.Read(animation.unknown);
	seekableReader.Read(animation.selectionRect);
	seekableReader.Read(animation.pixelDisplacement);
	seekableReader.Read(animation.unknown2);

	uint32_t frameCount;
	seekableReader.Read(frameCount);
	animation.frames.resize(frameCount);

	for (uint32_t i = 0; i < frameCount; ++i) {
		animation.frames[i] = ReadFrame(seekableReader);
	}

	seekableReader.Read<uint32_t>(animation.unknownContainer);

	return animation;
}

Animation::Frame ArtFile::ReadFrame(Stream::SeekableReader& seekableReader) {
	Animation::Frame frame;
	frame.optional1 = 0;
	frame.optional2 = 0;
	frame.optional3 = 0;
	frame.optional4 = 0;

	LayerMetadata layerMetadata;
	seekableReader.Read(layerMetadata);

	UnknownBitfield unknownBitfield;
	seekableReader.Read(unknownBitfield);

	frame.unknown = unknownBitfield.unknown;

	if (layerMetadata.bReadOptionalData) {
		seekableReader.Read(frame.optional1);
		seekableReader.Read(frame.optional2);
	}
	if (unknownBitfield.bReadOptionalData) {
		seekableReader.Read(frame.optional3);
		seekableReader.Read(frame.optional4);
	}

	frame.layers.resize(layerMetadata.layerCount);
	seekableReader.Read(frame.layers);

	return frame;
}

void ArtFile::VerifyFrameCount(const ArtFile& artFile, std::size_t frameCount, std::size_t layerCount, std::size_t unknownCount)
{
	std::size_t actualFrameCount = 0;
	std::size_t actualLayerCount = 0;
	std::size_t actualUnknownCount = 0;
	artFile.CountFrames(actualFrameCount, actualLayerCount, actualUnknownCount);

	if (actualFrameCount != frameCount) {
		throw std::runtime_error("Frame count does not match");
	}

	if (actualLayerCount != layerCount) {
		throw std::runtime_error("Sub-frame count does not match.");
	}

	// Need to figure out what unknown count is counting before enabling.
	//if (actualUnknownCount != unknownCount) {
	//	throw std::runtime_error("Unknown count does not match.");
	//}
}
