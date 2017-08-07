#pragma once

#include <string>
#include <vector>
#include <regex>

using namespace std;

// Cross platform file system access.
class XFile
{
public:
	//Returns the file extension as a std::string including the leading dot (.). 
	//If no extension is available, an empty string is returned. 
	static string getFileExtension(const string& pathStr);

	//Checks if the extension on a path matches the provided extension.
	//Provided extension does not require the leading dot (.).
	static bool extensionMatches(const string& pathStr, const string& extension);

	//Returns true if path is a directory WITHOUT a file included.
	static bool isDirectory(const string& pathStr);

	static vector<string> getFilesFromDirectory(const string& directory);

	static vector<string> getFilesFromDirectory(const string& directory, const string& fileType);

	static vector<string> getFilesFromDirectory(const string& directory, const regex& filenameRegex);

	static string changeFileExtension(const string& filename, const string& newExtension);

	static void createDirectory(const string& newPath);

	static bool pathExists(const string& pathStr);

	// Adds a string to the end of the filenameStr, but before the file's extension.
	static string appendToFilename(const string& filename, const string& valueToAppend);

	// Appends a subdirectory right before the filenameStr.
	static string appendSubDirectory(const string& pathStr, const string& subDirectory);

	static string getFilename(const string& pathStr);

	static string removeFilename(const string& pathStr);

	static bool pathsAreEquivalent(string path1, string path2);

	static bool isRootPath(const string& pathStr);

	static string replaceFilename(const string& pathStr, const string& filenameStr);
};
