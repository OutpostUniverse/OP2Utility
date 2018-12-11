#pragma once

#include <cstdint>

#pragma pack(push, 1) // Make sure structures are byte aligned

struct Rect
{
	int32_t x1;
	int32_t y1;
	int32_t x2;
	int32_t y2;

	inline int32_t Width() const {
		return x2 - x1;
	}

	inline int32_t Height() const {
		return y2 - y1;
	}

	bool operator==(const Rect& rhs) const {
		return (x1 == rhs.x1) && (y1 == rhs.y1) && (x2 == rhs.x2) && (y2 == rhs.y2);
	}
	bool operator!=(const Rect& rhs) const {
		return !(*this == rhs);
	}
};

static_assert(16 == sizeof(Rect), "Rect is an unexpected size");

#pragma pack(pop)
