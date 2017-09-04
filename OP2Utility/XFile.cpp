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
	if (pathStr.length() == 0)
		return true;

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

bool XFile::pathExists(const string& pathStr)
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

vector<string> XFile::getFilesFromDirectory(const string& directory)
{
	// Brett208 6Aug17: Creating a path with an empty string will prevent the directory_iterator from finding files in the current relative path.
	auto pathStr = directory.length() > 0 ? directory : "./";

	vector<string> filenames;
	for (auto& entry : directory_iterator(pathStr))
		filenames.push_back(entry.path().string());

	return filenames;
}

vector<string> XFile::getFilesFromDirectory(const string& directory, const regex& filenameRegex)
{
	vector<string> filenames = getFilesFromDirectory(directory);

	for (int i = filenames.size() - 1; i >= 0; i--) 
	{
		if (!regex_search(filenames[i], filenameRegex))
			filenames.erase(filenames.begin() + i);
	}

	return filenames;
}

vector<string> XFile::getFilesFromDirectory(const string& directory, const string& fileType)
{
	vector<string> filenames = getFilesFromDirectory(directory);

	for (int i = filenames.size() - 1; i >= 0; i--)
	{
		if (path(filenames[i]).extension() != fileType)
			filenames.erase(filenames.begin() + i);
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

bool XFile::pathsAreEqual(string pathStr1, string pathStr2)
{
	StringHelper::convertToUpper(pathStr1);
	StringHelper::convertToUpper(pathStr2);

	path path1(pathStr1);
	if (path1.has_relative_path() && path1.relative_path() == path1.filename())
		path1 = ("./" + pathStr1);
	
	path path2(pathStr2);
	if (path2.has_relative_path() && path2.relative_path() == path2.filename())
		path2 = ("./" + pathStr2);

	return path1 == path2;
}

string XFile::getDirectory(const string& pathStr)
{
	path p(pathStr);

	if (p.has_relative_path())
		if (p.relative_path() == p.filename())
			return "./";
		return p.relative_path().string();

	if (p.has_root_path())
		return p.root_path().string();

	return "./";
}

void XFile::deletePath(const string& pathStr)
{
	remove_all(pathStr);
}