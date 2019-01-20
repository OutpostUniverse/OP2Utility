#pragma once

#include <cstdint>
#include <vector>
#include <array>

// Placeholder for unknown object
struct ObjectType1
{
	std::array<uint8_t, 512> data;
};

// Placeholder struct for unit data
struct UnitRecord
{
	std::array<uint8_t, 120> data;
};

// Second section of saved game specifc data (not included in .map files)
struct SavedGameDataSection2
{
	uint32_t unitCount;
	uint32_t unknown1;
	uint32_t unknown2;
	uint32_t unknown3;
	uint32_t sizeOfUnit;
	uint32_t objectCount1;
	uint32_t objectCount2;
	std::vector<ObjectType1> objects1;
	std::vector<uint32_t> objects2;
	uint32_t unitID1;
	uint32_t unitID2;
	std::array<UnitRecord, 2047> unitRecord;
};
