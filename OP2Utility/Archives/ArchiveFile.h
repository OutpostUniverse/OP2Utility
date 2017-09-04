#pragma once

#include <windows.h>
#include "../StreamReader.h"

namespace Archives
{
	class ArchiveUnpacker
	{
	public:
		ArchiveUnpacker(const char *fileName);
		virtual ~ArchiveUnpacker();

		const char* GetVolumeFileName() { return m_VolumeFileName; };
		int GetVolumeFileSize() { return m_VolumeFileSize; };
		int GetNumberOfPackedFiles() { return m_NumberOfPackedFiles; };
		bool ContainsFile(const char* fileName);

		virtual const char* GetInternalFileName(int index) = 0;
		// Returns -1 if internalFileName is not present in archive.
		virtual int GetInternalFileIndex(const char *internalFileName) = 0;
		virtual int GetInternalFileSize(int index) = 0;
		virtual int ExtractFile(int index, const char *fileName) = 0;
		virtual int ExtractAllFiles(const char* destDirectory);
		virtual SeekableStreamReader* OpenSeekableStreamReader(const char *internalFileName) = 0;
		virtual SeekableStreamReader* OpenSeekableStreamReader(int fileIndex) = 0;

	protected:
		char *m_VolumeFileName;
		int m_NumberOfPackedFiles;
		int m_VolumeFileSize;
	};


	class ArchivePacker
	{
	public:
		ArchivePacker();
		virtual ~ArchivePacker();

		// Repack is used to replace the old volume with a new volume created from the
		// files (in the current directory) that match the internal file names
		virtual bool Repack() = 0;
		// Create volume is used to create a new volume file with the files specified
		// in filesToPack and gives them internal volume names specified in internalNames.
		// Returns true if successful and false otherwise
		virtual bool CreateVolume(const char *volumeFileName, int numFilesToPack,
			const char **filesToPack, const char **internalNames) = 0;
	protected:
		int OpenOutputFile(const char *fileName);
		void CloseOutputFile();
		bool ReplaceFileWithFile(const char *fileToReplace, const char *newFile);

		HANDLE m_OutFileHandle;
	};

	class ArchiveFile : public ArchiveUnpacker, public ArchivePacker
	{
	public:
		ArchiveFile(const char *fileName) : ArchiveUnpacker(fileName) {};
		virtual ~ArchiveFile() {};
	};


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
