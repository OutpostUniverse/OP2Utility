#pragma once

#include "HuffLZ.h"
#include "MemoryMappedFile.h"
#include "ArchiveFile.h"
#include "CompressionType.h"
#include "../Streams/StreamWriter.h"
#include <windows.h>
#include <string>
#include <vector>
#include <memory>

namespace Archives
{
#define VOL_WRITE_SIZE 65536

	class VolFile : public ArchiveFile, public MemoryMappedFile
	{
	public:
		VolFile(const char *filename);
		~VolFile();

		// Internal file status
		std::string GetInternalFileName(int index);
		int GetInternalFileIndex(const char *internalFileName);
		CompressionType GetInternalCompressionCode(int index);
		int GetInternalFileSize(int index);

		// Extraction
		void ExtractFile(int fileIndex, const std::string& pathOut);
		std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(const char *internalFileName);
		std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(int fileIndex);

		// Volume Creation
		bool Repack();
		bool CreateArchive(std::string volumeFileName, std::vector<std::string> filesToPack);

	private:
		int GetInternalFileOffset(int index);
		int GetInternalFileNameOffset(int index);

		void ExtractFileUncompressed(size_t index, const std::string& filename);
		void ExtractFileLzh(size_t index, const std::string& filename);

#pragma pack(push, 1)
		struct IndexEntry
		{
			unsigned int fileNameOffset;
			unsigned int dataBlockOffset;
			int fileSize;
			CompressionType compressionType;
		};
#pragma pack(pop)

		struct CreateVolumeInfo
		{
			IndexEntry *indexEntry;
			int *fileNameLength;
			HANDLE *fileHandle;
			std::vector<std::string> filesToPack;
			std::vector<std::string> internalNames;
			int stringTableLength;
			int indexTableLength;
			int paddedStringTableLength;
			int paddedIndexTableLength;

			size_t fileCount() const
			{
				return filesToPack.size();
			}
		};

		int ReadTag(int offset, const char *tagText);
		void WriteTag(StreamWriter& volWriter, int length, const char *tagText);
		void CopyFileIntoVolume(StreamWriter& volWriter, HANDLE inputFile, int size);
		bool ReadVolHeader();
		void WriteVolume(const std::string& fileName, const CreateVolumeInfo& volInfo);
		void WriteFiles(StreamWriter& volWriter, const CreateVolumeInfo &volInfo);
		void WriteHeader(StreamWriter& volWriter, const CreateVolumeInfo &volInfo);
		bool PrepareHeader(CreateVolumeInfo &volInfo);
		bool OpenAllInputFiles(CreateVolumeInfo &volInfo);
		void CleanUpVolumeCreate(CreateVolumeInfo &volInfo);

		int m_NumberOfIndexEntries;
		char *m_StringTable;
		int m_HeaderLength;
		int m_StringTableLength;
		int m_ActualStringTableLength;
		int m_IndexTableLength;
		IndexEntry *m_Index;
	};
}
