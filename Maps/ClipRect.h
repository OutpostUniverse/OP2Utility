#pragma once

#pragma pack(push, 1) // Make sure structures are byte aligned

// Represents playable area of the map.
struct ClipRect
{
	int x1;
	int y1;
	int x2;
	int y2;
};

#pragma pack(pop)
