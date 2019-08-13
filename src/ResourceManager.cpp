#include "ResourceManager.h"
#include "Archive/VolFile.h"
#include "Archive/ClmFile.h"
#include "Stream/BidirectionalReader.h"
#include "XFile.h"

using namespace Archive;

ResourceManager::ResourceManager(const std::string& archiveDirectory) :
	resourceRootDir(archiveDirectory)
{
	if (!XFile::IsDirectory(archiveDirectory)) {
		throw std::runtime_error("Resource manager must be passed an archive directory.");
	}

	const auto volFilenames = GetFilesFromDirectory(".vol");

	for (const auto& volFilename : volFilenames) {
		ArchiveFiles.push_back(std::make_unique<VolFile>(XFile::Append(archiveDirectory, volFilename)));
	}

	const auto clmFilenames = GetFilesFromDirectory(".clm");

	for (const auto& clmFilename : clmFilenames) {
		ArchiveFiles.push_back(std::make_unique<ClmFile>(XFile::Append(archiveDirectory, clmFilename)));
	}
}

// First searches for resources loosely in provided directory.
// Then, if accessArhives = true, searches the preloaded archives for the resource.
std::unique_ptr<Stream::BidirectionalReader> ResourceManager::GetResourceStream(const std::string& filename, bool accessArchives)
{
	// Only fully relative paths are supported
	if (XFile::HasRootComponent(filename)) {
		throw std::runtime_error("ResourceManager only accepts fully relative paths. Refusing: " + filename);
	}

	// Relative paths are relative to resourceRootDir
	const std::string path = XFile::Append(resourceRootDir, filename);
	if (XFile::PathExists(path)) {
		return std::make_unique<Stream::FileReader>(path);
	}

	if (!accessArchives) {
		return nullptr;
	}

	for (const auto& archiveFile : ArchiveFiles)
	{

		if (archiveFile->Contains(filename)) {
			auto index = archiveFile->GetIndex(filename);
			return archiveFile->OpenStream(index);
		}
	}

	return nullptr;
}

std::vector<std::string> ResourceManager::GetAllFilenames(const std::string& filenameRegexStr, bool accessArchives)
{
	std::regex filenameRegex(filenameRegexStr, std::regex_constants::icase);

	auto filenames = GetFilesFromDirectory(filenameRegex);

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

std::vector<std::string> ResourceManager::GetAllFilenamesOfType(const std::string& extension, bool accessArchives)
{
	auto filenames = GetFilesFromDirectory(extension);

	if (!accessArchives) {
		return filenames;
	}

	for (const auto& archiveFile : ArchiveFiles)
	{
		for (std::size_t i = 0; i < archiveFile->GetCount(); ++i)
		{
			std::string internalFilename = archiveFile->GetName(i);

			if (XFile::ExtensionMatches(internalFilename, extension) && !IsDuplicateFilename(filenames, internalFilename)) {
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

bool ResourceManager::IsDuplicateFilename(std::vector<std::string>& currentFilenames, std::string filenameToCheck)
{
	// Brett: When called on a large loop of filenames (60 or more), this function will create a bottleneck.

	for (const auto& currentFilename : currentFilenames) {
		if (XFile::PathsAreEqual(XFile::GetFilename(currentFilename), filenameToCheck)) {
			return true;
		}
	}

	return false;
}

std::string ResourceManager::FindContainingArchivePath(const std::string& filename)
{
	for (const auto& archiveFile : ArchiveFiles)
	{
		if (archiveFile->Contains(filename)) {
			return archiveFile->GetArchiveFilename();
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
		archiveFilenames.push_back(archiveFile->GetArchiveFilename());
	}
	return archiveFilenames;
}

std::vector<std::string> ResourceManager::GetFilesFromDirectory(const std::string& fileExtension)
{
	auto directoryContents = XFile::GetFilenamesFromDirectory(resourceRootDir, fileExtension);
	XFile::EraseNonFilenames(directoryContents);

	return directoryContents;
}

std::vector<std::string> ResourceManager::GetFilesFromDirectory(const std::regex& filenameRegex)
{
	auto directoryContents = XFile::GetFilenamesFromDirectory(resourceRootDir, filenameRegex);
	XFile::EraseNonFilenames(directoryContents);

	return directoryContents;
}
