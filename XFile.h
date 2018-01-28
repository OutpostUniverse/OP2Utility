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
	static std::string GetFileExtension(const std::string& pathStr);

	//Checks if the extension on a path matches the provided extension.
	//Provided extension does not require the leading dot (.).
	static bool ExtensionMatches(const std::string& pathStr, const std::string& extension);

	//Returns true if path is a directory WITHOUT a file included.
	static bool IsDirectory(const std::string& pathStr);

	static bool XFile::IsFile(const std::string& path);

	static std::vector<std::string> GetFilesFromDirectory(const std::string& directory);

	static std::vector<std::string> GetFilesFromDirectory(const std::string& directory, const std::string& fileType);

	static std::vector<std::string> GetFilesFromDirectory(const std::string& directory, const std::regex& filenameRegex);

	static std::string ChangeFileExtension(const std::string& filename, const std::string& newExtension);

	static void NewDirectory(const std::string& newPath);

	static bool PathExists(const std::string& pathStr);

	// Adds a string to the end of the filenameStr, but before the file's extension.
	static std::string AppendToFilename(const std::string& filename, const std::string& valueToAppend);

	// Appends a subdirectory right before the filenameStr.
	static std::string AppendSubDirectory(const std::string& pathStr, const std::string& subDirectory);

	static std::string GetFilename(const std::string& pathStr);

	static std::string RemoveFilename(const std::string& pathStr);

	static bool PathsAreEqual(std::string pathStr1, std::string pathStr2);

	static bool IsRootPath(const std::string& pathStr);

	static std::string ReplaceFilename(const std::string& pathStr, const std::string& filenameStr);

	static std::string GetDirectory(const std::string& pathStr);

	static void DeletePath(const std::string& pathStr);
};
