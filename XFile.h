#pragma once

#include <string>
#include <vector>
#include <regex>

// Cross platform file system access.
namespace XFile
{
	//Returns the file extension as a std::string including the leading dot (.). 
	//If no extension is available, an empty string is returned. 
	std::string GetFileExtension(const std::string& pathStr);

	//Checks if the extension on a path matches the provided extension.
	//Provided extension does not require the leading dot (.).
	bool ExtensionMatches(const std::string& pathStr, const std::string& extension);

	//Returns true if path is a directory WITHOUT a file included.
	bool IsDirectory(const std::string& pathStr);

	bool IsFile(const std::string& path);

	std::vector<std::string> GetFilesFromDirectory(const std::string& directory);

	std::vector<std::string> GetFilesFromDirectory(const std::string& directory, const std::string& fileType);

	std::vector<std::string> GetFilesFromDirectory(const std::string& directory, const std::regex& filenameRegex);

	std::string ChangeFileExtension(const std::string& filename, const std::string& newExtension);

	void NewDirectory(const std::string& newPath);

	bool PathExists(const std::string& pathStr);

	// Adds a string to the end of the filenameStr, but before the file's extension.
	std::string AppendToFilename(const std::string& filename, const std::string& valueToAppend);

	// Appends a subdirectory right before the filenameStr.
	std::string AppendSubDirectory(const std::string& pathStr, const std::string& subDirectory);

	std::string GetFilename(const std::string& pathStr);

	std::string RemoveFilename(const std::string& pathStr);

	bool PathsAreEqual(std::string pathStr1, std::string pathStr2);

	bool IsRootPath(const std::string& pathStr);

	std::string ReplaceFilename(const std::string& pathStr, const std::string& filenameStr);

	std::string GetDirectory(const std::string& pathStr);

	void DeletePath(const std::string& pathStr);

	void RenameFile(const std::string& oldPath, const std::string& newPath);
}
