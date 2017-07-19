#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "CHuffLZ.h"
#include "CArchiveFile.h"
#include "../StreamReader.h"

#define VOL_WRITE_SIZE 65536


class CVolFile : public CArchiveFile, public CMemoryMappedFile
{
	public:
		CVolFile(const char *filename);
		~CVolFile();

		// Internal file status
		const char* GetInternalFileName(int index);
		int GetInternalFileIndex(const char *internalFileName);
		short GetInternalCompressionCode(int index);
		int GetInternalFileSize(int index);

		// Extraction
		int ExtractFile(int index, const char *filename);
		SeekableStreamReader* OpenSeekableStreamReader(const char *internalFileName);
		SeekableStreamReader* OpenSeekableStreamReader(int fileIndex);

		// Volume Creation
		bool Repack();
		bool CreateVolume(const char *volumeFileName, int numFilesToPack,
						const char **filesToPack, const char **internalNames);

	private:
		int GetInternalFileOffset(int index);
		int GetInternalFileNameOffset(int index);		

		#pragma pack(push, 1)
		struct SIndexEntry
		{
			unsigned int fileNameOffset;
			unsigned int dataBlockOffset;
			int fileSize;
			short compressionTag;
		};
		#pragma pack(pop)

		struct SCreateVolumeInfo
		{
			SIndexEntry *indexEntry;
			int *fileNameLength;
			HANDLE *fileHandle;
			int numFilesToPack;
			const char **filesToPack;
			const char **internalNames;
			int stringTableLength;
			int indexTableLength;
			int paddedStringTableLength;
			int paddedIndexTableLength;
		};

		int ReadTag(int offset, const char *tagText);
		bool WriteTag(int length, const char *tagText);
		bool CopyFileIntoVolume(HANDLE inputFile, int size);
		bool ReadVolHeader();
		bool WriteFiles(SCreateVolumeInfo &volInfo);
		bool WriteHeader(SCreateVolumeInfo &volInfo);
		bool PrepareHeader(SCreateVolumeInfo &volInfo);
		bool OpenAllInputFiles(SCreateVolumeInfo &volInfo);
		void CleanUpVolumeCreate(SCreateVolumeInfo &volInfo);

		int m_NumberOfIndexEntries;
		char *m_StringTable;
		int m_HeaderLength;
		int m_StringTableLength;
		int m_ActualStringTableLength;
		int m_IndexTableLength;
		SIndexEntry *m_Index;
};
