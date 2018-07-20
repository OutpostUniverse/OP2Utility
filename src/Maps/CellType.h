#pragma once

// CellTypes returned and set by the GameMap class
enum class CellType
{
	FastPassible1 = 0,	// Rock vegetation
	Impassible2,		// Meteor craters, cracks/crevases
	SlowPassible1,		// Lava rock (dark)
	SlowPassible2,		// Rippled dirt/Lava rock bumps
	MediumPassible1,	// Dirt
	MediumPassible2,	// Lava rock
	Impassible1,		// Dirt/Rock/Lava rock mound/ice/volcano
	FastPassible2,		// Rock
	NorthCliffs,
	CliffsHighSide,
	CliffsLowSide,      // Cliffs, low and middle sections
	VentsAndFumaroles,	// Fumaroles (passable by GeoCons)
	zPad12,
	zPad13,
	zPad14,
	zPad15,
	zPad16,
	zPad17,
	zPad18,
	zPad19,
	zPad20,
	DozedArea,			// Bulldozed, non tubed area (buildings are tubed underneath). Does not include buildings.
	Rubble,
	NormalWall,
	MicrobeWall,
	LavaWall,
	Tube0,				// Used for tubes and areas under buildings
	Tube1,				// Note: Tube values 1-5 don't appear to be used
	Tube2,
	Tube3,
	Tube4,
	Tube5,
};