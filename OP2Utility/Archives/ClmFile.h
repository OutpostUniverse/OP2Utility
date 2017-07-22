#pragma once

#include <windows.h>
#include <mmeapi.h>
#include "ArchiveFile.h"
#include "../StreamReader.h"

namespace Archives
{
#define CLM_WRITE_SIZE 0x00020000

#define RIFF 0x46464952		// "RIFF"
#define WAVE 0x45564157		// "WAVE"
#define FMT  0x20746D66		// "fmt "
#define DATA 0x61746164		// "data"

	class ClmFile : public ArchiveFile
	{
	public:
		ClmFile(const char *fileName);
		~ClmFile();

		const char* GetInternalFileName(int index);
		int GetInternalFileIndex(const char *internalFileName);
		int ExtractFile(int index, const char *fileName);
		SeekableStreamReader* OpenSeekableStreamReader(const char *internalFileName);
		SeekableStreamReader* OpenSeekableStreamReader(int fileIndex);

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
