#pragma once

#include <cstdint>
#include <vector>
#include <array>

static const uint32_t DefaultSizeOfUnit = 120;

// Placeholder for unknown object
struct ObjectType1
{
	std::array<uint8_t, 512> data;
};

// Placeholder struct for unit data
struct UnitRecord
{
	std::array<uint8_t, DefaultSizeOfUnit> data;
};

// Second section of saved game specifc data (not included in .map files)
struct SavedGameUnits
{
	uint32_t unitCount;
	uint32_t lastUsedUnitIndex;
	uint32_t nextFreeUnitSlotIndex;
	uint32_t firstFreeUnitSlotIndex;
	uint32_t sizeOfUnit;
	uint32_t objectCount1;
	uint32_t objectCount2;
	std::vector<ObjectType1> objects1;
	std::vector<uint32_t> objects2;
	uint32_t nextUnitIndex; //Type UnitID
	uint32_t prevUnitIndex; //Type UnitID
	std::array<UnitRecord, 2047> units; // Was 1023 before patch
	std::array<uint32_t, 2048> freeUnits;

	void CheckSizeOfUnit() const;
};
