#include "StringHelper.h"
#include <algorithm>

using namespace std;

void StringHelper::ConvertToUpper(string& str)
{
	for (auto & c : str) c = toupper(c);
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

	for (size_t i = 0; i < stringVector.size(); ++i)
	{
		filenames[i] = stringVector[i].c_str();
	}

	return filenames;
}

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
