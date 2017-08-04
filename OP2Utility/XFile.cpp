#include "XFile.h"
#include "StringHelper.h"

#include <filesystem>
#include <experimental/filesystem>
using namespace std::experimental::filesystem;

string XFile::getFileExtension(const string& pathStr)
{
	return path(pathStr).extension().string();
}

bool XFile::isDirectory(const string& pathStr)
{
	return is_directory(pathStr);
}

bool XFile::extensionMatches(const string& pathStr, const string& extension)
{
	string pathExtension = getFileExtension(pathStr);
	StringHelper::convertToUpper(pathExtension);

	string extensionUpper = StringHelper::convertToUpper(extension);

	if (extensionUpper.length() > 0 && extensionUpper[0] != '.')
		extensionUpper.insert(0, ".");

	return pathExtension == extensionUpper;
}

string XFile::changeFileExtension(const string& filename, const string& newExtension)
{
	return path(filename).replace_extension(newExtension).string();
}

void XFile::createDirectory(const string& newPath)
{
	create_directory(path(newPath));
}

bool XFile::PathExists(const string& pathStr)
{
	return exists(path(pathStr));
}

string XFile::appendToFilename(const string& filename, const string& valueToAppend)
{
	path newPath(filename);
	
	path newFilename = newPath.filename().replace_extension("");
	newFilename += valueToAppend + newPath.extension().string();

	newPath.replace_filename(newFilename);
	
	return newPath.string();
}

vector<string> XFile::getFilesFromDirectory(const string& directoryStr, const regex& filenameRegex)
{
	path directory(path(directoryStr).remove_filename());

	// Brett208 4Aug17: creating a path with an empty string will prevent the directory_iterator from finding any files in the current relative path on MSVC.
	if (directoryStr == "")
		directory = path(".");

	vector<string> filenames;

	for (auto& directoryEntry : directory_iterator(directory))
	{
		string filename = directoryEntry.path().filename().string();
		if (regex_search(filename, filenameRegex))
			filenames.push_back(directoryEntry.path().string());
	}

	return filenames;
}

vector<string> XFile::getFilesFromDirectory(const string& directoryStr, const string& fileType)
{
	path directory(path(directoryStr).remove_filename());

	// Brett208 4Aug17: creating a path with an empty string will prevent the directory_iterator from finding any files in the current relative path on MSVC.
	if (directoryStr == "")
		directory = path(".");

	vector<string> filenames;

	for (auto& directoryEntry : directory_iterator(directory))
	{
		if (directoryEntry.path().extension() == fileType )
			filenames.push_back(directoryEntry.path().string());
	}

	return filenames;
}

bool XFile::isRootPath(const string& pathStr)
{
	return path(pathStr).has_root_path();
}

string XFile::replaceFilename(const string& pathStr, const string& filenameStr)
{
	path p(pathStr);
	path filename = path(filenameStr).filename();
	
	// Brett208 22JUL17: There seems to be a bug in path.replace_filename that removes a directory if it has a space in it on MSVC.

	return p.string() + "/" + filename.string();
}

string XFile::appendSubDirectory(const string& pathStr, const string& subDirectory)
{
	path p(pathStr);
	path filename(p.filename());

	if (p == filename)
		return path().append(subDirectory).append(filename).string();

	p = p.remove_filename();
	return p.append(subDirectory).append(filename).string();
}

string XFile::getFilename(const string& pathStr)
{
	return path(pathStr).filename().string();
}

string XFile::removeFilename(const string& pathStr)
{
	return path(pathStr).remove_filename().string();
}

bool XFile::pathsAreEqual(const string& pathStr1, const string& pathStr2)
{
	path p1(pathStr1);
	path p2(pathStr2);

	return p1 == p2;
}