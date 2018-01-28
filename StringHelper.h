#pragma once

#include <string>
#include <vector>

class StringHelper
{
public:
	static void ConvertToUpper(std::string& str);
	static std::string ConvertToUpper(const std::string& str);
	static const char** VectorToCharArray(const std::vector<std::string>& stringVector);
	static std::vector<std::string> RemoveMatchingStrings(const std::vector<std::string>& strings, const std::vector<std::string>& stringsToRemove);
};
