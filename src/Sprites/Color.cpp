#include "Color.h"

bool operator==(const Color& lhs, const Color& rhs) {
	return (lhs.red == rhs.red) &&
		(lhs.green == rhs.green) &&
		(lhs.blue == rhs.blue) &&
		(lhs.alpha == rhs.alpha);
}

bool operator!=(const Color& lhs, const Color& rhs) {
	return !operator==(lhs, rhs);
}
