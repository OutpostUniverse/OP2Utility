#include "StringHelper.h"
#include <algorithm>

using namespace std;

void StringHelper::ConvertToUpper(string& str)
{
	for (auto & c : str) {
		c = toupper(c);
	}
}

string StringHelper::ConvertToUpper(const string& str)
{
	string newString = str;
	ConvertToUpper(newString);

	return newString;
}

const char** StringHelper::VectorToCharArray(const vector<string>& stringVector)
{
	const char** filenames = new const char*[stringVector.size()];

	for (size_t i = 0; i < stringVector.size(); ++i) {
		filenames[i] = stringVector[i].c_str();
	}

	return filenames;
}

// Returns a new vector with matching strings removed. Case insensitive.
vector<string> StringHelper::RemoveMatchingStrings(const vector<string>& strings, const vector<string>& stringsToRemove)
{
	vector<string> stringsToReturn(strings.begin(), strings.end());

	auto pred = [&stringsToRemove](const std::string& key) ->bool
	{
		return std::find(stringsToRemove.begin(), stringsToRemove.end(), key) != stringsToRemove.end();
	};

	stringsToReturn.erase(std::remove_if(stringsToReturn.begin(), stringsToReturn.end(), pred), stringsToReturn.end());

	return stringsToReturn;
}

// Returns true if the vector contains the given string, ignoring letter casing.
bool StringHelper::ContainsStringCaseInsensitive(vector<string> stringsToSearch, string stringToFind)
{
	auto itr = std::find_if(stringsToSearch.begin(), stringsToSearch.end(),
		[&](auto &s) {

		if (s.size() != stringToFind.size()) {
			return false;
		}

		for (size_t i = 0; i < s.size(); ++i) {
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
