#pragma once

#include <cstdint>

#pragma pack(push, 1) // Make sure structures are byte aligned

struct Rect
{
	int32_t x1;
	int32_t y1;
	int32_t x2;
	int32_t y2;
};

static_assert(16 == sizeof(Rect), "Rect is an unexpected size");

#pragma pack(pop)
