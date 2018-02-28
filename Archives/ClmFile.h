#pragma once

#include "ArchiveFile.h"
#include <windows.h>
#include <mmreg.h>	// WAVEFORMATEX (omitted from windows.h if #define WIN32_LEAN_AND_MEAN)
#include <memory>

namespace Archives
{
	class ClmFile : public ArchiveFile
	{
	public:
		ClmFile(const char *fileName);
		virtual ~ClmFile();

		const char* GetInternalFileName(int index);
		int GetInternalFileIndex(const char *internalFileName);
		int ExtractFile(int index, const char *fileName);
		std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(const char *internalFileName);
		std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(int fileIndex);

		int GetInternalFileSize(int index);

		bool Repack();
		bool CreateVolume(const char *volumeFileName, int numFilesToPack,
			const char **filesToPack, const char **internalNames);

	private:

#pragma pack(push, 1)
		struct IndexEntry
		{
			char fileName[8];
			unsigned int dataOffset;
			unsigned int dataLength;
		};
#pragma pack(pop)

		// Private functions for reading archive
		bool ReadHeader();

		// Private functions for packing files
		bool OpenAllInputFiles(int numFilesToPack, const char **filesToPack, HANDLE *fileHandle);
		bool ReadAllWaveHeaders(int numFilesToPack, HANDLE *file, WAVEFORMATEX *format, IndexEntry *indexEntry);
		int FindChunk(int chunkTag, HANDLE file);
		void CleanUpVolumeCreate(HANDLE outFile, int numFilesToPack, HANDLE *fileHandle, WAVEFORMATEX *waveFormat, IndexEntry *indexEntry);
		bool CompareWaveFormats(int numFilesToPack, WAVEFORMATEX *waveFormat);
		bool WriteVolume(HANDLE outFile, int numFilesToPack, HANDLE *fileHandle,
			IndexEntry *entry, const char **internalName, WAVEFORMATEX *waveFormat);

		HANDLE m_FileHandle;
		WAVEFORMATEX m_WaveFormat;
		char m_Unknown[6];
		IndexEntry *m_IndexEntry;
		char(*m_FileName)[9];
	};
}
