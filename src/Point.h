#pragma once

#include <cstdint>

#pragma pack(push, 1) // Make sure structures are byte aligned

struct Point32
{
	int32_t x;
	int32_t y;
};

static_assert(8 == sizeof(Point32), "Point32 is an unexpected size");

struct Point16
{
	int16_t x;
	int16_t y;
};

static_assert(4 == sizeof(Point16), "Point16 is an unexpected size");

#pragma pack(pop)