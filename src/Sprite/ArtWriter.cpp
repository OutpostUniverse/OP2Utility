#include "ArtFile.h"
#include "PaletteHeader.h"
#include "../Stream/FileWriter.h"
#include <cstdint>
#include <stdexcept>
#include <algorithm>

void ArtFile::Write(std::string filename) const
{
	Stream::FileWriter artWriter(filename);
	Write(artWriter);
}

void ArtFile::Write(Stream::Writer& writer) const
{
	ValidateImageMetadata();

	WritePalettes(writer);

	writer.Write<uint32_t>(imageMetas);

	WriteAnimations(writer);
}

void ArtFile::WritePalettes(Stream::Writer& writer) const
{
	if (palettes.size() > UINT32_MAX) {
		throw std::runtime_error("Art file contains too many palettes.");
	}

	writer.Write(SectionHeader(TagPalette, static_cast<uint32_t>(palettes.size())));

	// Intentially do not pass palette as reference to allow local modification
	// Switch red and blue color to match Outpost 2 custom format.
	for (auto pallete : palettes) {
		writer.Write(PaletteHeader::CreatePaletteHeader());

		for (auto& color : pallete) {
			std::swap(color.red, color.blue);
		}

		writer.Write(pallete);
	}
}

void ArtFile::WriteAnimations(Stream::Writer& writer) const
{
	if (animations.size() > UINT32_MAX) {
		throw std::runtime_error("There are too many animations contained in the ArtFile.");
	}

	writer.Write(static_cast<uint32_t>(animations.size()));

	std::size_t frameCount;
	std::size_t layerCount;
	std::size_t unknownCount;
	CountFrames(frameCount, layerCount, unknownCount);

	if (frameCount > UINT32_MAX) {
		throw std::runtime_error("There are too many frames to write to file.");
	}

	if (layerCount > UINT32_MAX) {
		throw std::runtime_error("There are too many layers to write to file.");
	}

	if (unknownCount > UINT32_MAX) {
		throw std::runtime_error("There are too many unknown container items to write to file.");
	}

	writer.Write(static_cast<uint32_t>(frameCount));
	writer.Write(static_cast<uint32_t>(layerCount));
	writer.Write(static_cast<uint32_t>(unknownCount));

	for (const auto& animation : animations)
	{
		WriteAnimation(writer, animation);
	}
}

void ArtFile::WriteAnimation(Stream::Writer& writer, const Animation& animation)
{
	writer.Write(animation.unknown);
	writer.Write(animation.selectionRect);
	writer.Write(animation.pixelDisplacement);
	writer.Write(animation.unknown2);

	std::size_t frameCount = animation.frames.size();
	if (frameCount > UINT32_MAX) {
		throw std::runtime_error("There are too many frames in animation to write");
	}
	writer.Write(static_cast<uint32_t>(frameCount));

	for (const auto& frame : animation.frames) {
		WriteFrame(writer, frame);
	}

	writer.Write<uint32_t>(animation.unknownContainer);
}

void ArtFile::WriteFrame(Stream::Writer& writer, const Animation::Frame& frame)
{
	if (frame.layerMetadata.count != frame.layers.size()) {
		throw std::runtime_error("Recorded layer count must match number of written layers.");
	}

	writer.Write(frame.layerMetadata);
	writer.Write(frame.unknownBitfield);

	if (frame.layerMetadata.bReadOptionalData) {
		writer.Write(frame.optional1);
		writer.Write(frame.optional2);
	}

	if (frame.unknownBitfield.bReadOptionalData) {
		writer.Write(frame.optional3);
		writer.Write(frame.optional4);
	}

	writer.Write(frame.layers);
}
