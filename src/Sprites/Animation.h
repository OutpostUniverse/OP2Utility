#pragma once

#include "../Point.h"
#include "../Rect.h"
#include <vector>
#include <cstdint>

struct Animation {
	struct Frame {
		struct SubFrame {
			uint16_t bitmapIndex;
			uint8_t unknown; // Unused by Outpost 2
			uint8_t frameIndex;
			Point16 pixelOffset;
		};

		uint8_t unknown;
		uint8_t optional1;
		uint8_t optional2;
		uint8_t optional3;
		uint8_t optional4;

		// Limited to size of uint16_t MAX
		std::vector<SubFrame> subFrames;
	};

	struct UnknownContainer {
		uint32_t unknown1;
		uint32_t unknown2;
		uint32_t unknown3;
		uint32_t unknown4;
	};

	uint32_t unknown;
	Rect selectionRect; // pixels
	Point32 pixelDisplacement; // Reverse direction from an offset, origin is center of tile
	uint32_t unknown2; //(0x3C CC/DIRT/Garage/Std. Lab construction/dock, 0x0D spider walking)

	std::vector<Frame> frames;

	std::vector<UnknownContainer> unknownContainer;
};
