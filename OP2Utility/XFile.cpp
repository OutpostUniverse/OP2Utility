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

void XFile::GetFilesFromDirectory(vector<string>& filenamesOut, const string& pathStr, const string& fileType)
{
	//directory_iterator directoryIterator(path(pathStr));
	//directory_entry directoryEntry;

	//directory_iterator end;

	//for (directory_iterator iter(path(pathStr)); iter != end; iter)
	//{
	//	filenames.insert(iter.);
	//}

	//while (directoryIterator.increment(directoryEntry))
	//{

	//}
	//for (directory_iterator next(path("abc")), end; next != end; ++next) 

	//visit(next->path());
}