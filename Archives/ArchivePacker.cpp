#include "ArchivePacker.h"
#include "../XFile.h"
#include "../StringHelper.h"
#include <stdexcept>

namespace Archives
{
	ArchivePacker::ArchivePacker()
	{
		m_OutFileHandle = nullptr;
	}

	ArchivePacker::~ArchivePacker()
	{
		if (m_OutFileHandle) CloseHandle(m_OutFileHandle);
	}

	// Opens the file for output and returns nonzero if successful
	int ArchivePacker::OpenOutputFile(const char *fileName)
	{
		m_OutFileHandle = CreateFileA(fileName,		// filename
			GENERIC_WRITE,			// desired access
			0,						// share mode
			nullptr,				// security attributes
			CREATE_ALWAYS,			// creation disposition
			FILE_ATTRIBUTE_NORMAL,	// attributes
			nullptr);				// template

		if (m_OutFileHandle == INVALID_HANDLE_VALUE) {
			return 0;
		}
		else {
			return 1;
		}
	}

	// Closes the file whose handle is in m_OutFileHandle
	void ArchivePacker::CloseOutputFile()
	{
		if (m_OutFileHandle) CloseHandle(m_OutFileHandle);
		m_OutFileHandle = nullptr;
	}

	// newFile has it's name changed to that of fileToReplace and the old file is deleted.
	// Returns true if successful and false otherwise
	bool ArchivePacker::ReplaceFileWithFile(const char *fileToReplace, const char *newFile)
	{
		MoveFileA(fileToReplace, "Delete.vol");
		if (MoveFileA(newFile, fileToReplace) == 0) return false;
		DeleteFileA("Delete.vol");

		return true;
	}

	std::vector<std::string> ArchivePacker::GetInternalNamesFromPaths(std::vector<std::string> paths)
	{
		std::vector<std::string> fileNames;

		for (std::string fileName : paths) {
			if (StringHelper::ContainsStringCaseInsensitive(fileNames, fileName)) {
				throw std::runtime_error("Unable to create an archive containing multiple files with the same filename.");
			}

			fileNames.push_back(XFile::GetFilename(fileName));
		}

		return fileNames;
	}

	bool ArchivePacker::ComparePathFilenames(const std::string path1, const std::string path2)
	{
		return StringHelper::StringCompareCaseInsensitive(XFile::GetFilename(path1), XFile::GetFilename(path2));
	}
}
