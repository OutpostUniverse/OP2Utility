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
