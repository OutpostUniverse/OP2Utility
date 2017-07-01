
#ifndef C_ARCHIVE_FILE
#define C_ARCHIVE_FILE

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


class CArchiveUnpacker
{
	public:
		CArchiveUnpacker(const char *fileName);
		~CArchiveUnpacker();

		const char* GetVolumeFileName() { return m_VolumeFileName; };
		int GetVolumeFileSize()			{ return m_VolumeFileSize; };
		int GetNumberOfPackedFiles()	{ return m_NumberOfPackedFiles; };

		virtual const char* GetInternalFileName(int index) = 0;
		virtual int GetInternalFileSize(int index) = 0;
		virtual int ExtractFile(int index, const char *fileName) = 0;
	protected:
		char *m_VolumeFileName;
		int m_NumberOfPackedFiles;
		int m_VolumeFileSize;
};


class CArchivePacker
{
	public:
		CArchivePacker();
		~CArchivePacker();

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

class CArchiveFile : public CArchiveUnpacker, public CArchivePacker
{
	public:
		CArchiveFile(const char *fileName) : CArchiveUnpacker(fileName) {};
};


class CMemoryMappedFile
{
	protected:
		CMemoryMappedFile();
		~CMemoryMappedFile();

		int MemoryMapFile(const char *fileName);
		void UnmapFile();

		HANDLE m_FileHandle;
		HANDLE m_FileMappingHandle;
		LPVOID m_BaseOfFile;
		int m_MappedFileSize;
};

#endif