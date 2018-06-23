#include "StringHelper.h"
#include <algorithm>
#include <cstddef>

void StringHelper::ConvertToUpper(std::string& str)
{
	for (auto & c : str) {
		c = toupper(c);
	}
}

std::string StringHelper::ConvertToUpper(const std::string& str)
{
	std::string newString = str;
	ConvertToUpper(newString);

	return newString;
}

// Returns a new vector with matching strings removed. Case insensitive.
std::vector<std::string> StringHelper::RemoveStrings(const std::vector<std::string>& stringsToSearch, const std::vector<std::string>& stringsToRemove)
{
	std::vector<std::string> stringsToReturn(stringsToSearch);

	// i will wrap around to SIZE_MAX  when loop is completed
	for (std::size_t i = stringsToSearch.size() - 1; i < stringsToSearch.size(); ++i) {
		for (const auto& stringToRemove : stringsToRemove) {
			if (CheckIfStringsAreEqual(stringsToSearch[i], stringToRemove)) {
				stringsToReturn.erase(stringsToReturn.begin() + i);
				break;
			}
		}
	}

	return stringsToReturn;
}

bool StringHelper::CheckIfStringsAreEqual(const std::string& string1, const std::string& string2)
{
	if (string1.size() != string2.size()) {
		return false;
	}

	for (std::size_t i = 0; i < string1.size(); ++i) {
		if (::tolower(string1[i]) != ::tolower(string2[i])) {
			return false;
		}
	}

	return true;
}

// Returns true if the vector contains the given string, ignoring letter casing.
bool StringHelper::ContainsStringCaseInsensitive(std::vector<std::string> stringsToSearch, std::string stringToFind)
{
	auto itr = std::find_if(stringsToSearch.begin(), stringsToSearch.end(),
		[&](auto &s) {

		if (s.size() != stringToFind.size()) {
			return false;
		}

		for (std::size_t i = 0; i < s.size(); ++i) {
			if (::tolower(s[i]) != ::tolower(stringToFind[i])) {
				return false;
			}
		}
		return true;
	}
	);

	return itr != stringsToSearch.end();
}

// Compares 2 strings without considering casing of letters. Compatible with std library sort functions.
// This function is required because built in comparison between 2 characters defaults to an uppercase 
// letter being higher precedent than ANY lowercase letter. 
bool StringHelper::StringCompareCaseInsensitive(const std::string& string1, const std::string& string2)
{
	unsigned int i = 0;
	while ((i < string1.length()) && (i < string2.length()))
	{
		if (tolower(string1[i]) < tolower(string2[i])) {
			return true;
		}
		else if (tolower(string1[i]) > tolower(string2[i])) {
			return false;
		}

		++i;
	}

	return (string1.length() < string2.length());
}

bool StringHelper::ContainsNonAsciiChars(std::string str)
{
	return std::any_of(str.begin(), str.end(), [](unsigned char i) { return (static_cast<unsigned char>(i) > 127); });
}
