#include "ArtFile.h"
#include "PaletteHeader.h"
#include "../Stream/FileReader.h"
#include <stdexcept>
#include <cstdint>
#include <cstddef>
#include <algorithm>

ArtFile ArtFile::Read(std::string filename) {
	Stream::FileReader mapReader(filename);
	return Read(mapReader);
}

ArtFile ArtFile::Read(Stream::Reader& reader) {
	ArtFile artFile;

	ReadPalette(reader, artFile);
	ReadImageMetadata(reader, artFile);
	ReadAnimations(reader, artFile);

	return artFile;
}

// Read ArtFile from an rvalue stream (unnamed temporary)
ArtFile ArtFile::Read(Stream::Reader&& reader) {
	// Delegate to lvalue overload
	return Read(reader);
}

void ArtFile::ReadPalette(Stream::Reader& reader, ArtFile& artFile)
{
	SectionHeader paletteSectionHeader;
	reader.Read(paletteSectionHeader);
	paletteSectionHeader.Validate(TagPalette);

	artFile.palettes.resize(paletteSectionHeader.length);

	for (uint32_t i = 0; i < paletteSectionHeader.length; ++i) {
		PaletteHeader paletteHeader;
		reader.Read(paletteHeader);

		paletteHeader.Validate();

		reader.Read(artFile.palettes[i]);

		// Rearrange color into standard format. Outpost 2 uses custom color order.
		for (auto& color : artFile.palettes[i])
		{
			std::swap(color.red, color.blue);
		}
	}
}

void ArtFile::ReadImageMetadata(Stream::Reader& reader, ArtFile& artFile)
{
	reader.Read<uint32_t>(artFile.imageMetas);

	artFile.ValidateImageMetadata();
}

void ArtFile::ReadAnimations(Stream::Reader& reader, ArtFile& artFile)
{
	uint32_t animationCount;
	reader.Read(animationCount);
	artFile.animations.resize(animationCount);

	uint32_t frameCount;
	reader.Read(frameCount);

	uint32_t layerCount;
	reader.Read(layerCount);

	reader.Read(artFile.unknownAnimationCount);

	for (uint32_t i = 0; i < animationCount; ++i)
	{
		artFile.animations[i] = ReadAnimation(reader);
	}

	VerifyCountsMatchHeader(artFile, frameCount, layerCount, artFile.unknownAnimationCount);
}

Animation ArtFile::ReadAnimation(Stream::Reader& reader)
{
	Animation animation;

	reader.Read(animation.unknown);
	reader.Read(animation.selectionRect);
	reader.Read(animation.pixelDisplacement);
	reader.Read(animation.unknown2);

	uint32_t frameCount;
	reader.Read(frameCount);
	animation.frames.resize(frameCount);

	for (uint32_t i = 0; i < frameCount; ++i) {
		animation.frames[i] = ReadFrame(reader);
	}

	reader.Read<uint32_t>(animation.unknownContainer);

	return animation;
}

Animation::Frame ArtFile::ReadFrame(Stream::Reader& reader) {
	Animation::Frame frame;
	frame.optional1 = 0;
	frame.optional2 = 0;
	frame.optional3 = 0;
	frame.optional4 = 0;

	reader.Read(frame.layerMetadata);
	reader.Read(frame.unknownBitfield);

	if (frame.layerMetadata.bReadOptionalData) {
		reader.Read(frame.optional1);
		reader.Read(frame.optional2);
	}
	if (frame.unknownBitfield.bReadOptionalData) {
		reader.Read(frame.optional3);
		reader.Read(frame.optional4);
	}

	frame.layers.resize(frame.layerMetadata.count);
	reader.Read(frame.layers);

	return frame;
}

void ArtFile::VerifyCountsMatchHeader(const ArtFile& artFile, std::size_t frameCount, std::size_t layerCount, std::size_t unknownCount)
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

	if (actualUnknownCount != unknownCount) {
		throw std::runtime_error("Unknown count does not match.");
	}
}
