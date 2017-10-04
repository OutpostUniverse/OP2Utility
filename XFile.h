#pragma once

#include <string>
#include <vector>
#include <regex>

// Cross platform file system access.
class XFile
{
public:
	//Returns the file extension as a std::string including the leading dot (.). 
	//If no extension is available, an empty string is returned. 
	static std::string getFileExtension(const std::string& pathStr);

	//Checks if the extension on a path matches the provided extension.
	//Provided extension does not require the leading dot (.).
	static bool extensionMatches(const std::string& pathStr, const std::string& extension);

	//Returns true if path is a directory WITHOUT a file included.
	static bool isDirectory(const std::string& pathStr);

	static bool XFile::isFile(const std::string& path);

	static std::vector<std::string> getFilesFromDirectory(const std::string& directory);

	static std::vector<std::string> getFilesFromDirectory(const std::string& directory, const std::string& fileType);

	static std::vector<std::string> getFilesFromDirectory(const std::string& directory, const std::regex& filenameRegex);

	static std::string changeFileExtension(const std::string& filename, const std::string& newExtension);

	static void createDirectory(const std::string& newPath);

	static bool pathExists(const std::string& pathStr);

	// Adds a string to the end of the filenameStr, but before the file's extension.
	static std::string appendToFilename(const std::string& filename, const std::string& valueToAppend);

	// Appends a subdirectory right before the filenameStr.
	static std::string appendSubDirectory(const std::string& pathStr, const std::string& subDirectory);

	static std::string getFilename(const std::string& pathStr);

	static std::string removeFilename(const std::string& pathStr);

	static bool pathsAreEqual(std::string pathStr1, std::string pathStr2);

	static bool isRootPath(const std::string& pathStr);

	static std::string replaceFilename(const std::string& pathStr, const std::string& filenameStr);

	static std::string getDirectory(const std::string& pathStr);

	static void deletePath(const std::string& pathStr);
};
