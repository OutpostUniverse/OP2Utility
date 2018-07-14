#pragma once

#include <string>
#include <vector>

class StringHelper
{
public:
	static void ConvertToUpper(std::string& str);
	static std::string ConvertToUpper(const std::string& str);
	static std::vector<std::string> RemoveStrings(std::vector<std::string> stringsToSearch, const std::vector<std::string>& stringsForRemoval);
	static bool CheckIfStringsAreEqual(const std::string& string1, const std::string& string2);
	static bool ContainsStringCaseInsensitive(std::vector<std::string> stringsToSearch, std::string stringToFind);
	static bool StringCompareCaseInsensitive(const std::string& string1, const std::string& string2);
	static bool ContainsNonAsciiChars(std::string str);
};
