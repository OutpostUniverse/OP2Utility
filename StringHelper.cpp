#include "StringHelper.h"
#include <algorithm>

void StringHelper::convertToUpper(string& str)
{
	for (auto & c : str) c = toupper(c);
}

string StringHelper::convertToUpper(const string& str)
{
	string newString = str;
	convertToUpper(newString);

	return newString;
}

const char** StringHelper::vectorToCharArray(const vector<string>& stringVector)
{
	const char** filenames = new const char*[stringVector.size()];

	for (size_t i = 0; i < stringVector.size(); ++i)
	{
		filenames[i] = stringVector[i].c_str();
	}

	return filenames;
}

vector<string> StringHelper::removeMatchingStrings(const vector<string>& strings, const vector<string>& stringsToRemove)
{
	vector<string> stringsToReturn(strings.begin(), strings.end());

	auto pred = [&stringsToRemove](const std::string& key) ->bool
	{
		return std::find(stringsToRemove.begin(), stringsToRemove.end(), key) != stringsToRemove.end();
	};

	stringsToReturn.erase(std::remove_if(stringsToReturn.begin(), stringsToReturn.end(), pred), stringsToReturn.end());

	return stringsToReturn;
}