#pragma once

#include <string>
#include <vector>

class StringHelper
{
public:
	static void convertToUpper(std::string& str);
	static std::string convertToUpper(const std::string& str);
	static const char** vectorToCharArray(const std::vector<std::string>& stringVector);
	static std::vector<std::string> removeMatchingStrings(const std::vector<std::string>& strings, const std::vector<std::string>& stringsToRemove);
};