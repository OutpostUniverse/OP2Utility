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

static_assert(4 == sizeof(Color), "Color is an unexpected size");

#pragma pack(pop)

bool operator==(const Color& lhs, const Color& rhs);
bool operator!=(const Color& lhs, const Color& rhs);

using Palette = std::array<Color, 256>;

namespace DiscreteColor
{
	const Color Black{ 0, 0, 0, 0 };
	const Color White{ 255, 255, 255,0 };

	const Color Red{ 255, 0, 0, 0 };
	const Color Green{ 0, 255, 0, 0 };
	const Color Blue{ 0, 0, 255, 0 };

	const Color Yellow{ 255, 255, 0 };
	const Color Cyan{ 0, 255, 255 };
	const Color Magenta{ 255, 0, 255 };

	const Color TransparentBlack{ 0, 0, 0, 255 };
	const Color TransparentWhite{ 255, 255, 255, 255 };
}
