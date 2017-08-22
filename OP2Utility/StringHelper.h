#pragma once

#include <string>
#include <vector>

using namespace std;

class StringHelper
{
public:
	static void convertToUpper(string& str);
	static string convertToUpper(const string& str);
	static const char** vectorToCharArray(const vector<string>& stringVector);
};