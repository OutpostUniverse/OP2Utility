#pragma once

#include <string>
#include <vector>

using namespace std;

// Cross platform file system access.
class XFile
{
public:
	//Returns the file extension as a std::string including the leading dot (.). 
	//If no extension is available, an empty string is returned. 
	static string GetFileExtension(string pathStr);

	//Checks if the extension on a path matches the provided extension.
	//Provided extension does not require the leading dot (.).
	static bool ExtensionMatches(const string& pathStr, const string& extension);

	//Returns true if path is a directory WITHOUT a file included.
	static bool IsDirectory(const string& pathStr);

	//Brett: I've been defeated trying to get the iterator working on this function.
	static void GetFilesFromDirectory(vector<string>& filenamesOut, const string& pathStr, const string& fileType);

	static string ChangeFileExtension(string filename, string newExtension);

private:
	static void ConvertToUpper(string& str);
	static string XFile::ConvertToUpper(const string& str);
};
