#include"XFile.h"

#if defined(_WIN32)
#include <filesystem>
#include <experimental\filesystem>
using namespace std::experimental::filesystem;
#endif

void XFile::ConvertToUpper(string& str)
{
	for (auto & c : str) c = toupper(c);
}

string XFile::ConvertToUpper(const string& str)
{
	string newString = str;
	ConvertToUpper(newString);

	return newString;
}

string XFile::GetFileExtension(string pathStr)
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

bool XFile::IsDirectory(const string& pathStr)
{
#if defined(_WIN32)
	return is_directory(pathStr);
#else
#error Operating system not supported.
#endif
}

bool XFile::ExtensionMatches(const string& pathStr, const string& extension)
{
#if defined(_WIN32)
	string pathExtension = GetFileExtension(pathStr);
	ConvertToUpper(pathExtension);

	string extensionUpper = ConvertToUpper(extension);

	if (extensionUpper.length() > 0 && extensionUpper[0] != '.')
		extensionUpper.insert(0, ".");

	return pathExtension == extensionUpper;
#else
#error Operating system not supported.
#endif
}

string XFile::ChangeFileExtension(const string& filename, const string& newExtension)
{
#if defined(_WIN32)
	return path(filename).replace_extension(newExtension).string();
#endif
}

void XFile::CreateDirectory(const string& newPath)
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

string XFile::AppendToFilename(const string& filename, const string& valueToAppend)
{
#if defined(_WIN32)
	path newPath(filename);
	
	path newFilename = newPath.filename().replace_extension("");
	newFilename += valueToAppend;
	newFilename.replace_extension(newPath.extension());

	newPath.replace_filename(newFilename);
	
	return newPath.string();
#endif
}

void XFile::GetFilesFromDirectory(vector<string>& filenamesOut, const string& pathStr, const string& fileType)
{
#if defined(_WIN32)
	path p(pathStr);

	if (pathStr == "" || pathStr == "/" || pathStr == "\\" || pathStr == " ")
		p = current_path();

	for (auto& directoryEntry : directory_iterator(p))
	{
		if (directoryEntry.path().extension() == fileType )
			filenamesOut.push_back(directoryEntry.path().string());
	}
#endif
}

string XFile::AppendSubDirectory(const string& pathStr, const string& subDirectory)
{
#if defined(_WIN32)
	path p(pathStr);
	path filename(p.filename());
	p.remove_filename();
	return p.append(subDirectory).append(filename).string();
#endif
}