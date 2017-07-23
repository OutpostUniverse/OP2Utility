#include"XFile.h"
#include "StringHelper.h"

#if defined(_WIN32)
#include <filesystem>
#include <experimental\filesystem>
using namespace std::experimental::filesystem;
#endif

string XFile::getFileExtension(string pathStr)
{
#if defined(_WIN32)
	return path(pathStr).extension().string();

#elif defined(linux)
	// TODO: Allow this function to distinguish between periods placed in the filename versus a folder on Linux.
#error Operating system not supported.

#elif defined(APPLE)
#error Operating system not supported.

#else
#error Operating system not supported.
#endif
}

bool XFile::isDirectory(const string& pathStr)
{
#if defined(_WIN32)
	return is_directory(pathStr);
#else
#error Operating system not supported.
#endif
}

bool XFile::extensionMatches(const string& pathStr, const string& extension)
{
#if defined(_WIN32)
	string pathExtension = getFileExtension(pathStr);
	StringHelper::convertToUpper(pathExtension);

	string extensionUpper = StringHelper::convertToUpper(extension);

	if (extensionUpper.length() > 0 && extensionUpper[0] != '.')
		extensionUpper.insert(0, ".");

	return pathExtension == extensionUpper;
#else
#error "Operating system not supported."
#endif
}

string XFile::changeFileExtension(const string& filename, const string& newExtension)
{
#if defined(_WIN32)
	return path(filename).replace_extension(newExtension).string();
#endif
}

void XFile::newDirectory(const string& newPath)
{
#if defined(_WIN32)
	create_directory(path(newPath));
#endif
}

bool XFile::PathExists(const string& pathStr)
{
#if defined(_WIN32)
	return exists(path(pathStr));
#endif
}

string XFile::appendToFilename(const string& filename, const string& valueToAppend)
{
#if defined(_WIN32)
	path newPath(filename);
	
	path newFilename = newPath.filename().replace_extension("");
	newFilename += valueToAppend + newPath.extension().string();
	//newFilename.replace_extension(newPath.extension());

	newPath.replace_filename(newFilename);
	
	return newPath.string();
#endif
}

void XFile::getFilesFromDirectory(vector<string>& filenamesOut, const string& pathStr, const string& fileType)
{
#if defined(_WIN32)
	path p(pathStr);
	path directory(p.remove_filename());
	if (p == directory)
		directory = current_path();

	if (pathStr == "" || pathStr == "/" || pathStr == "\\" || pathStr == " ")
		directory = current_path();

	for (auto& directoryEntry : directory_iterator(directory))
	{
		if (directoryEntry.path().extension() == fileType )
			filenamesOut.push_back(directoryEntry.path().string());
	}
#endif
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
#if defined(_WIN32)
	path p(pathStr);
	path filename(p.filename());

	if (p == filename)
		return path().append(subDirectory).append(filename).string();

	p = p.remove_filename();
	return p.append(subDirectory).append(filename).string();
#endif
}

string XFile::getFilename(const string& pathStr)
{
#if defined(_WIN32)
	path p(pathStr);
	return p.filename().string();
#endif
}

string XFile::removeFilename(const string& pathStr)
{
#if defined (_WIN32)
	path p(pathStr);
	return p.remove_filename().string();
#endif
}

bool XFile::pathsAreEqual(const string& pathStr1, const string& pathStr2)
{
#if defined (_WIN32)
	path p1(pathStr1);
	path p2(pathStr2);

	return p1 == p2;
#endif
}