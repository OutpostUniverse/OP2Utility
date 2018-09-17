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

namespace DiscreteColor
{
	const Color Black{ 0, 0, 0, 0 };
	const Color White{ 255, 255, 255,0 };
	const Color Red{ 255, 0, 0, 0 };
	const Color Green{ 0, 255, 0, 0 };
	const Color Blue{ 0, 0, 255, 0 };
}
