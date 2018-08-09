#pragma once

#include <cstdint>

#pragma pack(push, 1) // Make sure structures are byte aligned

// Represents playable area of the map.
struct ClipRect
{
	int32_t x1;
	int32_t y1;
	int32_t x2;
	int32_t y2;
};

#pragma pack(pop)