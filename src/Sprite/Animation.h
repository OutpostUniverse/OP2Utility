#pragma once

#include "../Point.h"
#include "../Rect.h"
#include <vector>
#include <cstdint>

#pragma pack(push, 1) // Make sure structures are byte aligned

struct Animation {
	struct Frame {
		struct LayerMetadata {
			uint8_t count : 7;
			uint8_t bReadOptionalData : 1;
		};

		static_assert(1 == sizeof(LayerMetadata), "Animation::Frame::LayerMetadata is an unexpected size");

		struct Layer {
			uint16_t bitmapIndex;
			uint8_t unknown; // Unused by Outpost 2
			uint8_t frameIndex;
			Point16 pixelOffset;
		};

		static_assert(8 == sizeof(Layer), "Animation::Frame::Layer is an unexpected size");

		LayerMetadata layerMetadata;
		LayerMetadata unknownBitfield;

		uint8_t optional1;
		uint8_t optional2;
		uint8_t optional3;
		uint8_t optional4;

		// Maximum count of items in container is 128
		std::vector<Layer> layers;
	};

	struct UnknownContainer {
		uint32_t unknown1;
		uint32_t unknown2;
		uint32_t unknown3;
		uint32_t unknown4;
	};

	static_assert(16 == sizeof(UnknownContainer), "Animation::UnknownContainer is an unexpected size");

	uint32_t unknown;
	Rect selectionRect; // pixels
	Point32 pixelDisplacement; // Reverse direction from an offset, origin is center of tile
	uint32_t unknown2; //(0x3C CC/DIRT/Garage/Std. Lab construction/dock, 0x0D spider walking)

	std::vector<Frame> frames;

	std::vector<UnknownContainer> unknownContainer;
};

#pragma pack(pop)
