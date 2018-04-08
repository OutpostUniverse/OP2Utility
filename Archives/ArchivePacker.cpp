#include "ArchivePacker.h"

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
}
