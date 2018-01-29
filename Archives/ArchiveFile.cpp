#include "ArchiveFile.h"
#include "../Xfile.h"

namespace Archives
{
	ArchiveUnpacker::ArchiveUnpacker(const char *fileName)
	{
		// Copy the filename to class storage
		m_VolumeFileName = new char[strlen(fileName) + 1];// Allocate space for the filename
		strcpy(m_VolumeFileName, fileName);				// Copy the filename to class storage

		m_NumberOfPackedFiles = 0;
		m_VolumeFileSize = 0;
	}

	ArchiveUnpacker::~ArchiveUnpacker()
	{
		delete m_VolumeFileName;
	}

	int ArchiveUnpacker::ExtractAllFiles(const char* destDirectory)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			if (ExtractFile(i, XFile::ReplaceFilename(destDirectory, GetInternalFileName(i)).c_str()) == false) {
				return false;
			}
		}

		return true;
	}

	bool ArchiveUnpacker::ContainsFile(const char* fileName)
	{
		for (int i = 0; i < GetNumberOfPackedFiles(); ++i)
		{
			if (XFile::PathsAreEqual(GetInternalFileName(i), fileName))
				return true;
		}

		return false;
	}

	// **************************************************************************************
	// **************************************************************************************
	// **************************************************************************************
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


	// **************************************************************************************
	// **************************************************************************************
	// **************************************************************************************
	// Initialize memory mapping variables
	MemoryMappedFile::MemoryMappedFile()
	{
		m_FileHandle = nullptr;
		m_FileMappingHandle = nullptr;
		m_BaseOfFile = nullptr;
		m_MappedFileSize = 0;
	}

	// Release the memory mapping (if it exists)
	MemoryMappedFile::~MemoryMappedFile()
	{
		if (m_BaseOfFile) UnmapViewOfFile(m_BaseOfFile);
		if (m_FileMappingHandle) CloseHandle(m_FileMappingHandle);
		if (m_FileHandle) CloseHandle(m_FileHandle);
	}

	// Opens and maps a view of the specified file.
	// Returns 0 on success and nonzero on failure.
	int MemoryMappedFile::MemoryMapFile(const char *fileName)
	{
		// Open the file
		m_FileHandle = CreateFileA(fileName,			// filename
			GENERIC_READ,			// access mode
			FILE_SHARE_READ,		// share mode
			nullptr,				// security attributes
			OPEN_EXISTING,			// creation disposition
			FILE_ATTRIBUTE_NORMAL,	// file attributes
			0);						// template
// Check for errors opening file
		if (m_FileHandle == INVALID_HANDLE_VALUE)
		{
			// Error opening file
			return 1;
		}
		// Store the file size
		m_MappedFileSize = GetFileSize(m_FileHandle, nullptr);

		// Create a file mapping object
		m_FileMappingHandle = CreateFileMapping(m_FileHandle,	// file handle
			nullptr,				// security attributes
			PAGE_READONLY,			// flProtect
			0,						// size high
			0,						// size low
			nullptr);				// name of mapping
// Map the entire file into memory
		m_BaseOfFile = MapViewOfFile(m_FileMappingHandle,		// file mapping handle
			FILE_MAP_READ,			// desired access
			0,						// offset high
			0,						// offset low
			0);						// number of bytes to map

		return 0;	// Return success
	}

	// Release the memory mapping (if it exists)
	void MemoryMappedFile::UnmapFile()
	{
		if (m_BaseOfFile) UnmapViewOfFile(m_BaseOfFile);
		if (m_FileMappingHandle) CloseHandle(m_FileMappingHandle);
		if (m_FileHandle) CloseHandle(m_FileHandle);

		m_BaseOfFile = nullptr;
		m_FileMappingHandle = nullptr;
		m_FileHandle = nullptr;
		m_MappedFileSize = 0;
	}
}
