#include "XFile.h"
#include "StringHelper.h"
#include <cstddef>

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

std::string XFile::GetFileExtension(const std::string& pathStr)
{
	return fs::path(pathStr).extension().string();
}

bool XFile::IsDirectory(const std::string& pathStr)
{
	if (pathStr.length() == 0) {
		return true;
	}

	return fs::is_directory(pathStr);
}

bool XFile::IsFile(const std::string& path)
{
	return fs::is_regular_file(path);
}

bool XFile::ExtensionMatches(const std::string& pathStr, const std::string& extension)
{
	std::string pathExtension = GetFileExtension(pathStr);
	StringHelper::ConvertToUpper(pathExtension);

	std::string extensionUpper = StringHelper::ConvertToUpper(extension);

	if (extensionUpper.length() > 0 && extensionUpper[0] != '.') {
		extensionUpper.insert(0, ".");
	}

	return pathExtension == extensionUpper;
}

std::string XFile::ChangeFileExtension(const std::string& filename, const std::string& newExtension)
{
	return fs::path(filename).replace_extension(newExtension).string();
}

void XFile::NewDirectory(const std::string& newPath)
{
	fs::create_directory(fs::path(newPath));
}

bool XFile::PathExists(const std::string& pathStr)
{
	return fs::exists(fs::path(pathStr));
}

std::string XFile::AppendToFilename(const std::string& filename, const std::string& valueToAppend)
{
	fs::path newPath(filename);

	fs::path newFilename = newPath.filename().replace_extension("");
	newFilename += valueToAppend + newPath.extension().string();

	newPath.replace_filename(newFilename);

	return newPath.string();
}

std::vector<std::string> XFile::GetFilesFromDirectory(const std::string& directory)
{
	// Brett208 6Aug17: Creating a path with an empty string will prevent the directory_iterator from finding files in the current relative path.
	auto pathStr = directory.length() > 0 ? directory : "./";

	std::vector<std::string> filenames;
	for (const auto& entry : fs::directory_iterator(pathStr)) {
		filenames.push_back(entry.path().string());
	}

	return filenames;
}

std::vector<std::string> XFile::GetFilesFromDirectory(const std::string& directory, const std::regex& filenameRegex)
{
	std::vector<std::string> filenames = GetFilesFromDirectory(directory);

	// Loop starts at index size - 1 and ends after index 0 executes
	for (std::size_t i = filenames.size(); i-- > 0; )
	{
		if (!std::regex_search(filenames[i], filenameRegex)) {
			filenames.erase(filenames.begin() + i);
		}
	}

	return filenames;
}

std::vector<std::string> XFile::GetFilesFromDirectory(const std::string& directory, const std::string& fileType)
{
	std::vector<std::string> filenames = GetFilesFromDirectory(directory);

	// Loop starts at index size - 1 and ends after index 0 executes
	for (std::size_t i = filenames.size(); i-- > 0; )
	{
		if (filenames.size() == 0) {
			return filenames;
		}

		std::string extension = fs::path(filenames[i]).extension().string();
		if (extension != fileType) {
			filenames.erase(filenames.begin() + i);
		}
	}

	return filenames;
}

bool XFile::IsRootPath(const std::string& pathStr)
{
	return fs::path(pathStr).has_root_path();
}

std::string XFile::ReplaceFilename(const std::string& pathStr, const std::string& filenameStr)
{
	fs::path p(pathStr);
	fs::path filename = fs::path(filenameStr).filename();

	// Brett208 22JUL17: There seems to be a bug in path.replace_filename that removes a directory if it has a space in it on MSVC.

	return p.string() + "/" + filename.string();
}

std::string XFile::AppendSubDirectory(const std::string& pathStr, const std::string& subDirectory)
{
	fs::path p(pathStr);
	fs::path filename(p.filename());

	if (p == filename) {
		return (fs::path() / subDirectory / filename).string();
	}

	p = p.remove_filename();
	return (p / subDirectory / filename).string();
}

std::string XFile::GetFilename(const std::string& pathStr)
{
	return fs::path(pathStr).filename().string();
}

std::string XFile::RemoveFilename(const std::string& pathStr)
{
	return fs::path(pathStr).remove_filename().string();
}

bool XFile::PathsAreEqual(std::string pathStr1, std::string pathStr2)
{
	StringHelper::ConvertToUpper(pathStr1);
	StringHelper::ConvertToUpper(pathStr2);

	fs::path path1(pathStr1);
	if (path1.has_relative_path() && path1.relative_path() == path1.filename()) {
		path1 = ("./" + pathStr1);
	}

	fs::path path2(pathStr2);
	if (path2.has_relative_path() && path2.relative_path() == path2.filename()) {
		path2 = ("./" + pathStr2);
	}

	return path1 == path2;
}

std::string XFile::GetDirectory(const std::string& pathStr)
{
	fs::path p(pathStr);

	if (p.has_relative_path())
	{
		if (p.relative_path() == p.filename()) {
			return "./";
		}

		return p.remove_filename().relative_path().string();
	}

	if (p.has_root_path()) {
		return p.remove_filename().root_path().string();
	}

	return "./";
}

void XFile::DeletePath(const std::string& pathStr)
{
	fs::remove_all(pathStr);
}

void XFile::RenameFile(const std::string& oldPath, const std::string& newPath)
{
	fs::rename(oldPath, newPath);
}
