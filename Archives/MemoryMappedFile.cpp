#include "MemoryMappedFile.h"

namespace Archives
{
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
