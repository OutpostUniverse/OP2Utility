#include "ArtFile.h"
#include "PaletteHeader.h"
#include "../Stream/FileWriter.h"
#include <cstdint>
#include <stdexcept>
#include <algorithm>

void ArtFile::Write(std::string filename, const ArtFile& artFile)
{
	Stream::FileWriter artWriter(filename);
	Write(artWriter, artFile);
}

void ArtFile::Write(Stream::BidirectionalSeekableWriter& seekableWriter, const ArtFile& artFile)
{
	artFile.ValidateImageMetadata();

	WritePalettes(seekableWriter, artFile);

	seekableWriter.Write<uint32_t>(artFile.imageMetas);

	WriteAnimations(seekableWriter, artFile);
}

void ArtFile::WritePalettes(Stream::BidirectionalSeekableWriter& seekableWriter, const ArtFile& artFile)
{
	if (artFile.palettes.size() > UINT32_MAX) {
		throw std::runtime_error("Art file contains too many palettes.");
	}

	seekableWriter.Write(SectionHeader(TagPalette, static_cast<uint32_t>(artFile.palettes.size())));

	// Intentially do not pass palette as reference to allow local modification
	// Switch red and blue color to match Outpost 2 custom format.
	for (auto pallete : artFile.palettes) {
		seekableWriter.Write(PaletteHeader(artFile));

		for (auto& color : pallete) {
			std::swap(color.red, color.blue);
		}

		seekableWriter.Write(pallete);
	}
}

void ArtFile::WriteAnimations(Stream::BidirectionalSeekableWriter& seekableWriter, const ArtFile& artFile)
{
	if (artFile.animations.size() > UINT32_MAX) {
		throw std::runtime_error("There are too many animations contained in the ArtFile.");
	}

	seekableWriter.Write(static_cast<uint32_t>(artFile.animations.size()));

	std::size_t frameCount;
	std::size_t layerCount;
	std::size_t unknownCount;
	artFile.CountFrames(frameCount, layerCount, unknownCount);

	if (frameCount > UINT32_MAX) {
		throw std::runtime_error("There are too many frames to write to file.");
	}

	if (layerCount > UINT32_MAX) {
		throw std::runtime_error("There are too many layers to write to file.");
	}

	if (unknownCount > UINT32_MAX) {
		throw std::runtime_error("There are too many unknown container items to write to file.");
	}

	seekableWriter.Write(static_cast<uint32_t>(frameCount));
	seekableWriter.Write(static_cast<uint32_t>(layerCount));
	seekableWriter.Write(static_cast<uint32_t>(unknownCount));

	for (const auto& animation : artFile.animations)
	{
		WriteAnimation(seekableWriter, animation);
	}
}

void ArtFile::WriteAnimation(Stream::BidirectionalSeekableWriter& seekableWriter, const Animation& animation)
{
	seekableWriter.Write(animation.unknown);
	seekableWriter.Write(animation.selectionRect);
	seekableWriter.Write(animation.pixelDisplacement);
	seekableWriter.Write(animation.unknown2);

	std::size_t frameCount = animation.frames.size();
	if (frameCount > UINT32_MAX) {
		throw std::runtime_error("There are too many frames in animation to write");
	}
	seekableWriter.Write(static_cast<uint32_t>(frameCount));

	for (const auto& frame : animation.frames) {
		WriteFrame(seekableWriter, frame);
	}

	seekableWriter.Write<uint32_t>(animation.unknownContainer);
}

void ArtFile::WriteFrame(Stream::BidirectionalSeekableWriter& seekableWriter, const Animation::Frame& frame)
{
	if (frame.layerMetadata.count != frame.layers.size()) {
		throw std::runtime_error("Recorded layer count must match number of written layers.");
	}

	seekableWriter.Write(frame.layerMetadata);
	seekableWriter.Write(frame.unknownBitfield);

	if (frame.layerMetadata.bReadOptionalData) {
		seekableWriter.Write(frame.optional1);
		seekableWriter.Write(frame.optional2);
	}

	if (frame.unknownBitfield.bReadOptionalData) {
		seekableWriter.Write(frame.optional3);
		seekableWriter.Write(frame.optional4);
	}

	seekableWriter.Write(frame.layers);
}
