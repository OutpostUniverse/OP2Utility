#pragma once

#include <array>
#include <cstdint>

struct Color {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

using Palette = std::array<Color, 256>;