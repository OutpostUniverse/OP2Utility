#pragma once

#include <cstdint>

enum class ImageType : uint16_t
{
	MenuGraphic = 0,
	GameGraphic = 1,
	UnitShadow1 = 4,
	UnitShadow2 = 5,
	Unknown1 = 33,
	TruckBed = 65,
	Unknown3 = 97,
	Unknown4 = 129
};
