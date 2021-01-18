// Includes functions and constexpr variables that are useful in multiple tileset
// source code files to prevent circular include references

#pragma once

#include "../Tag.h"
#include "../StringUtility.h"
#include <string>
#include <cstdint>
#include <type_traits>
#include <stdexcept>

namespace Tileset
{
	constexpr auto DefaultTagData = MakeTag("data");
	constexpr uint32_t DefaultPaletteHeaderSize = 1024;

	template<typename T>
	std::string formatReadErrorMessage(std::string propertyName, T value, T expectedValue)
	{
		using namespace StringUtility;
		return "Tileset property " + propertyName + " reads " + StringFrom(value) + ". Expected a value of " + StringFrom(expectedValue) + ".";
	}

	template<typename T>
	void throwReadError(std::string propertyName, T value, T expectedValue)
	{
		throw std::runtime_error(formatReadErrorMessage(propertyName, value, expectedValue));
	}
}
