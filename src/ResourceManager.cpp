#include "ResourceManager.h"
#include "Archive/VolFile.h"
#include "Archive/ClmFile.h"
#include "Stream/SeekableReader.h"
#include "XFile.h"
#include <regex>

using namespace Archives;

ResourceManager::ResourceManager(const std::string& archiveDirectory)
{
	auto volFilenames = XFile::GetFilesFromDirectory(archiveDirectory, ".vol");

	for (const auto& volFilename : volFilenames) {
		ArchiveFiles.push_back(std::make_unique<VolFile>(volFilename));
	}

	auto clmFilenames = XFile::GetFilesFromDirectory(archiveDirectory, ".clm");

	for (const auto& clmFilename : clmFilenames) {
		ArchiveFiles.push_back(std::make_unique<ClmFile>(clmFilename));
	}
}

// First searches for resources loosely in provided directory.
// Then, if accessArhives = true, searches the preloaded archives for the resource.
std::unique_ptr<Stream::SeekableReader> ResourceManager::GetResourceStream(const std::string& filename, bool accessArchives)
{
	if (XFile::PathExists(filename)) {
		return std::make_unique<Stream::FileReader>(filename);
	}

	if (!accessArchives) {
		return nullptr;
	}

	for (const auto& archiveFile : ArchiveFiles)
	{
		std::string internalArchiveFilename = XFile::GetFilename(filename);

		if (archiveFile->Contains(internalArchiveFilename)) {
			auto index = archiveFile->GetIndex(internalArchiveFilename);
			return archiveFile->OpenStream(index);
		}
	}

	return nullptr;
}

std::vector<std::string> ResourceManager::GetAllFilenames(const std::string& directory, const std::string& filenameRegexStr, bool accessArchives)
{
	std::regex filenameRegex(filenameRegexStr, std::regex_constants::icase);

	std::vector<std::string> filenames = XFile::GetFilesFromDirectory(directory, filenameRegex);

	if (!accessArchives) {
		return filenames;
	}

	for (const auto& archiveFile : ArchiveFiles)
	{
		for (std::size_t i = 0; i < archiveFile->GetCount(); ++i)
		{
			if (std::regex_search(archiveFile->GetName(i), filenameRegex)) {
				filenames.push_back(archiveFile->GetName(i));
			}
		}
	}

	return filenames;
}

std::vector<std::string> ResourceManager::GetAllFilenamesOfType(const std::string& directory, const std::string& extension, bool accessArchives)
{
	std::vector<std::string> filenames = XFile::GetFilesFromDirectory(directory, extension);

	if (!accessArchives) {
		return filenames;
	}

	for (const auto& archiveFile : ArchiveFiles)
	{
		for (std::size_t i = 0; i < archiveFile->GetCount(); ++i)
		{
			std::string internalFilename = archiveFile->GetName(i);

			if (XFile::ExtensionMatches(internalFilename, extension) && !DuplicateFilename(filenames, internalFilename)) {
				filenames.push_back(internalFilename);
			}
		}
	}

	return filenames;
}

bool ResourceManager::ExistsInArchives(const std::string& filename, std::size_t& archiveIndexOut, std::size_t& internalIndexOut)
{
	for (std::size_t i = 0; i < ArchiveFiles.size(); ++i)
	{
		for (std::size_t j = 0; j < ArchiveFiles[i]->GetCount(); ++j)
		{
			if (XFile::PathsAreEqual(ArchiveFiles[i]->GetName(j), filename))
			{
				archiveIndexOut = i;
				internalIndexOut = j;
				return true;
			}
		}
	}

	return false;
}

bool ResourceManager::ExtractSpecificFile(const std::string& filename, bool overwrite)
{
	if (!overwrite && XFile::PathExists(filename)) {
		return true;
	}

	std::size_t archiveIndex;
	std::size_t internalIndex;
	if (ExistsInArchives(filename, archiveIndex, internalIndex))
	{
		ArchiveFiles[archiveIndex]->ExtractFile(internalIndex, filename);
		return true;
	}

	return false;
}

void ResourceManager::ExtractAllOfFileType(const std::string& directory, const std::string& extension, bool overwrite)
{
	for (const auto& archiveFile : ArchiveFiles)
	{
		for (std::size_t i = 0; i < archiveFile->GetCount(); ++i)
		{
			if (XFile::ExtensionMatches(archiveFile->GetName(i), extension)) {
				archiveFile->ExtractFile(i, archiveFile->GetName(i));
			}
		}
	}
}

bool ResourceManager::DuplicateFilename(std::vector<std::string>& currentFilenames, std::string pathToCheck)
{
	// Brett: When called on a large loop of filenames (60 or more), this function will create a bottleneck.

	std::string filename = XFile::GetFilename(pathToCheck);

	for (const auto& currentFilename : currentFilenames) {
		if (XFile::PathsAreEqual(XFile::GetFilename(currentFilename), filename)) {
			return true;
		}
	}

	return false;
}

std::string ResourceManager::FindContainingArchiveFile(const std::string& filename)
{
	for (const auto& archiveFile : ArchiveFiles)
	{
		if (archiveFile->Contains(filename)) {
			return XFile::GetFilename(archiveFile->GetVolumeFilename());
		}
	}

	return std::string();
}

std::vector<std::string> ResourceManager::GetArchiveFilenames()
{
	std::vector<std::string> archiveFilenames;
	archiveFilenames.reserve(ArchiveFiles.size());

	for (const auto& archiveFile : ArchiveFiles)
	{
		archiveFilenames.push_back(archiveFile->GetVolumeFilename());
	}
	return archiveFilenames;
}
