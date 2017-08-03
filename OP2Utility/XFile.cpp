#include"XFile.h"
#include "StringHelper.h"

#if defined(_WIN32)
#include <filesystem>
#include <experimental/filesystem>
using namespace std::experimental::filesystem;
#endif

string XFile::getFileExtension(string pathStr)
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

void XFile::newDirectory(const string& newPath)
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
	path directory(directoryStr);

	if (directoryStr == "" || directoryStr == "/" || directoryStr == "\\" || directoryStr == " ")
		directory = current_path();

	vector<string> filenames;

	for (auto& directoryEntry : directory_iterator(directory))
	{
		string filename = directoryEntry.path().filename().string();
		if (regex_search(filename, filenameRegex))
			filenames.push_back(directoryEntry.path().string());
	}

	return filenames;
}

vector<string> XFile::getFilesFromDirectory(const string& pathStr, const string& fileType)
{
	vector<string> filenames;

	path p(pathStr);
	path directory(p.remove_filename());
	if (p == directory)
		directory = current_path();

	if (pathStr == "" || pathStr == "/" || pathStr == "\\" || pathStr == " ")
		directory = current_path();

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

string XFile::replaceFilename(const string& pathStr, const string& filename)
{
	path p(pathStr);
	path filenamePath = path(filename).filename();
	
	// Brett208 22JUL17: There seems to be a bug in path.replace_filename that removes a directory if it has a space in it.

	return p.string() + "\\" + filenamePath.string();
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