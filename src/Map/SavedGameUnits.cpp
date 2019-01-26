#include "SavedGameUnits.h"
#include <stdexcept>

void SavedGameUnits::CheckSizeOfUnit() const
{
	if (sizeOfUnit != 120 && unitCount != 0) {
		throw std::runtime_error("Size of unit must by 120 bytes if unit count is not 0");
	}
}
