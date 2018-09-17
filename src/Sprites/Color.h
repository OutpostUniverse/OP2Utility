#pragma once

#include <array>
#include <cstdint>

#pragma pack(push, 1) // Make sure structures are byte aligned

struct Color {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

#pragma pack(pop)

using Palette = std::array<Color, 256>;
