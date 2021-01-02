// Includes functions and constexpr variables that are useful in multiple tileset
// source code files to prevent circular include references

#pragma once

#include "../Tag.h"
#include <string>
#include <cstdint>

namespace Tileset
{
	constexpr auto DefaultTagData = MakeTag("data");
	constexpr uint32_t DefaultPaletteHeaderSize = 1024;

	void throwReadError(std::string propertyName, std::string value, std::string expectedValue);
	void throwReadError(std::string propertyName, uint32_t value, uint32_t expectedValue);

	std::string formatReadErrorMessage(std::string propertyName, std::string value, std::string expectedValue);
}
