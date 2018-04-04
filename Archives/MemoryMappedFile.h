#pragma once

#include <windows.h>

namespace Archives
{
	class MemoryMappedFile
	{
	protected:
		MemoryMappedFile();
		~MemoryMappedFile();

		int MemoryMapFile(const char *fileName);
		void UnmapFile();

		HANDLE m_FileHandle;
		HANDLE m_FileMappingHandle;
		LPVOID m_BaseOfFile;
		int m_MappedFileSize;
	};
}