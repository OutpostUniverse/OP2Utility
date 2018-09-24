#pragma once

#include <cstdint>

#pragma pack(push, 1) // Make sure structures are byte aligned

struct Point32
{
	int32_t x;
	int32_t y;
};

struct Point16
{
	int16_t x;
	int16_t y;
};

#pragma pack(pop)