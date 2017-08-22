#include "StringHelper.h"

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
