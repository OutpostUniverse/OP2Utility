#include "Map/SavedGameUnits.h"
#include <gtest/gtest.h>
#include <stdexcept>

using namespace OP2Utility;

TEST(SavedGameUnits, CheckSizeOfUnit) 
{
	SavedGameUnits savedGameUnits;
	savedGameUnits.unitCount = 0;
	savedGameUnits.sizeOfUnit = 0;

	// Does not throw for any unit size if unit count is 0;
	EXPECT_NO_THROW(savedGameUnits.CheckSizeOfUnit());

	// Throws if unit count is not 0 and unit size is not 120
	savedGameUnits.unitCount = 1;
	EXPECT_THROW(savedGameUnits.CheckSizeOfUnit(), std::runtime_error);

	savedGameUnits.sizeOfUnit = DefaultSizeOfUnit;
	EXPECT_NO_THROW(savedGameUnits.CheckSizeOfUnit());
}
