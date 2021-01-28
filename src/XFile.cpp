#include "XFile.h"
#include "StringUtility.h"
#include <cstddef>
#include <algorithm>

#ifdef __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

namespace XFile
{
std::string GetFileExtension(const std::string& pathStr)
{
	return fs::path(pathStr).extension().string();
}

bool IsDirectory(const std::string& pathStr)
{
	if (pathStr.length() == 0) {
		return true;
	}

	return fs::is_directory(pathStr);
}

bool IsFile(const std::string& path)
{
	return fs::is_regular_file(path);
}

bool ExtensionMatches(const std::string& pathStr, const std::string& extension)
{
	std::string pathExtension = GetFileExtension(pathStr);
	StringUtility::ConvertToUpperInPlace(pathExtension);

	std::string extensionUpper = StringUtility::ConvertToUpper(extension);

	if (extensionUpper.length() > 0 && extensionUpper[0] != '.') {
		extensionUpper.insert(0, ".");
	}

	return pathExtension == extensionUpper;
}

std::string ChangeFileExtension(const std::string& filename, const std::string& newExtension)
{
	return fs::path(filename).replace_extension(newExtension).string();
}

void NewDirectory(const std::string& newPath)
{
	fs::create_directory(fs::path(newPath));
}

bool PathExists(const std::string& pathStr)
{
	return fs::exists(fs::path(pathStr));
}

bool HasRootComponent(const std::string& pathStr)
{
	fs::path path(pathStr);
	return path.has_root_name() || path.has_root_directory();
}

std::string MakeAbsolute(const std::string& pathStr, const std::string& relativeRootDirStr)
{
	return HasRootComponent(pathStr) ? pathStr : Append(relativeRootDirStr, pathStr);
}

std::string Append(const std::string& path1, const std::string& relativePath2)
{
	fs::path nestedPath(relativePath2);
	if (nestedPath.has_root_name() || nestedPath.has_root_directory()) {
		throw std::runtime_error("Cannot append paths with root component: " + relativePath2);
	}
	return (fs::path(path1) / nestedPath).generic_string();
}

std::string AppendToFilename(const std::string& filename, const std::string& valueToAppend)
{
	fs::path newPath(filename);

	fs::path newFilename = newPath.filename().replace_extension("");
	newFilename += valueToAppend + newPath.extension().string();

	newPath.replace_filename(newFilename);

	return newPath.string();
}

std::vector<std::string> Dir(const std::string& directory)
{
	// Creating a path with an empty string will prevent the directory_iterator from finding files in the current relative path.
	auto pathStr = directory.length() > 0 ? directory : "./";

	std::vector<std::string> filenames;
	for (const auto& entry : fs::directory_iterator(pathStr)) {
		filenames.push_back(GetFilename(entry.path().generic_string()));
	}

	return filenames;
}

template <typename SelectFunction>
std::vector<std::string> DirInternal(const std::string& directory, SelectFunction selectFunction)
{
	// Creating a path with an empty string will prevent the directory_iterator from finding files in the current relative path.
	auto pathStr = directory.length() > 0 ? directory : "./";

	std::vector<std::string> filenames;
	for (const auto& entry : fs::directory_iterator(pathStr)) {
		auto pathString = entry.path().generic_string();
		if (selectFunction(pathString)) {
			filenames.push_back(GetFilename(pathString));
		}
	}

	return filenames;
}

std::vector<std::string> Dir(const std::string& directory, const std::regex& filenameRegex)
{
	return DirInternal(
		directory,
		[&filenameRegex](const std::string& filename) {
			return std::regex_search(filename, filenameRegex);
		}
	);
}

std::vector<std::string> DirWithExtension(const std::string& directory, const std::string& extension)
{
	return DirInternal(
		directory,
		[&extension](const std::string& filename) {
			return fs::path(filename).extension().string() == extension;
		}
	);
}

std::vector<std::string> DirFiles(const std::string& directory)
{
	return DirInternal(
		directory,
		[](const std::string& filename) {
			return IsFile(filename);
		}
	);
}

std::vector<std::string> DirFiles(const std::string& directory, const std::regex& filenameRegex)
{
	return DirInternal(
		directory,
		[&filenameRegex](const std::string& filename) {
			return std::regex_search(filename, filenameRegex) && IsFile(filename);
		}
	);
}

std::vector<std::string> DirFilesWithExtension(const std::string& directory, const std::string& extension)
{
	return DirInternal(
		directory,
		[&extension](const std::string& filename) {
			return (fs::path(filename).extension().string() == extension) && IsFile(filename);
		}
	);
}

void EraseNonFilenames(std::vector<std::string>& directoryContents)
{
	directoryContents.erase(
		std::remove_if(
			directoryContents.begin(),
			directoryContents.end(),
			[](std::string path) { return !IsFile(path); }
		),
		directoryContents.end()
	);
}

bool IsRootPath(const std::string& pathStr)
{
	return fs::path(pathStr).has_root_path();
}

std::string ReplaceFilename(const std::string& pathStr, const std::string& filenameStr)
{
	fs::path p(pathStr);

#if _WIN32 // MSVC will return unexpected results when passing only a filename into replace_filename
	if (p.filename() == pathStr) {
		return filenameStr;
	}
#endif

	return p.replace_filename(filenameStr).generic_string();
}

std::string AppendSubDirectory(const std::string& pathStr, const std::string& subDirectory)
{
	fs::path p(pathStr);
	fs::path filename(p.filename());

	if (p == filename) {
		return (fs::path() / subDirectory / filename).string();
	}

	p = p.remove_filename();
	return (p / subDirectory / filename).string();
}

std::string GetFilename(const std::string& pathStr)
{
	return fs::path(pathStr).filename().string();
}

bool PathsAreEqual(std::string pathStr1, std::string pathStr2)
{
	StringUtility::ConvertToUpperInPlace(pathStr1);
	StringUtility::ConvertToUpperInPlace(pathStr2);

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

std::string GetDirectory(const std::string& pathStr)
{
	if (pathStr.size() == 0) {
		return "";
	}
	if (pathStr.back() == '/') {
		return pathStr;
	}

	fs::path p(pathStr);
	auto returnPathStr = p.parent_path().generic_string();
	if (returnPathStr.size() > 0) {
		returnPathStr += "/";
	}
	return returnPathStr;
}

void DeletePath(const std::string& pathStr)
{
	fs::remove_all(pathStr);
}

void RenameFile(const std::string& oldPath, const std::string& newPath)
{
	fs::rename(oldPath, newPath);
}
}
