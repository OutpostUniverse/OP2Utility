#pragma once

#include <string>
#include <vector>
#include <type_traits>

namespace StringUtility
{
	void ConvertToUpperInPlace(std::string& str);
	std::string ConvertToUpper(std::string str);
	std::vector<std::string> RemoveStrings(std::vector<std::string> stringsToSearch, const std::vector<std::string>& stringsForRemoval);
	bool IsEqual(const std::string& string1, const std::string& string2);
	bool IsEqualCaseInsensitive(const std::string& string1, const std::string& string2);
	bool ContainsStringCaseInsensitive(std::vector<std::string> stringsToSearch, std::string stringToFind);
	bool ContainsNonAsciiChars(std::string str);

	template<typename T>
	std::string StringFrom(T value)
	{
		if constexpr (std::is_convertible<T, std::string>::value) {
			return value;
		}
		else if constexpr (std::is_same_v<T, bool>) {
			return value ? "true" : "false";
		}
		else {
			return std::to_string(value);
		}
	}
}
