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
	static string getFileExtension(string pathStr);

	//Checks if the extension on a path matches the provided extension.
	//Provided extension does not require the leading dot (.).
	static bool extensionMatches(const string& pathStr, const string& extension);

	//Returns true if path is a directory WITHOUT a file included.
	static bool isDirectory(const string& pathStr);

	static vector<string> getFilesFromDirectory(const string& pathStr, const string& fileType);

	static vector<string> getFilesFromDirectory(const string& directoryStr, const regex& filenameRegex);

	static string changeFileExtension(const string& filename, const string& newExtension);

	static void newDirectory(const string& newPath);

	static bool PathExists(const string& pathStr);

	// Adds a string to the end of the filename, but before the file's extension.
	static string XFile::appendToFilename(const string& filename, const string& valueToAppend);

	static string XFile::appendSubDirectory(const string& pathStr, const string& subDirectory);

	static string XFile::getFilename(const string& pathStr);

	static string XFile::removeFilename(const string& pathStr);

	static bool pathsAreEqual(const string& pathStr1, const string& pathStr2);

	static bool XFile::isRootPath(const string& pathStr);

	static string XFile::replaceFilename(const string& pathStr, const string& filename);
};
