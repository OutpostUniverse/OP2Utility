#pragma once

#include <string>
#include <vector>

namespace StringUtility
{
	void ConvertToUpper(std::string& str);
	std::string ConvertToUpper(const std::string& str);
	std::vector<std::string> RemoveStrings(std::vector<std::string> stringsToSearch, const std::vector<std::string>& stringsForRemoval);
	bool CheckIfStringsAreEqual(const std::string& string1, const std::string& string2);
	bool ContainsStringCaseInsensitive(std::vector<std::string> stringsToSearch, std::string stringToFind);
	bool StringCompareCaseInsensitive(const std::string& string1, const std::string& string2);
	bool ContainsNonAsciiChars(std::string str);
}
