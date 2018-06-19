#pragma once

#include "HuffLZ.h"
#include "ArchiveFile.h"
#include "CompressionType.h"
#include "../Streams/StreamWriter.h"
#include "../Streams/FileStreamReader.h"
#include <windows.h>
#include <cstddef>
#include <string>
#include <vector>
#include <array>
#include <memory>

namespace Archives
{
#define VOL_WRITE_SIZE 65536

	class VolFile : public ArchiveFile
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

		void ExtractFileUncompressed(std::size_t index, const std::string& filename);
		void ExtractFileLzh(std::size_t index, const std::string& filename);

#pragma pack(push, 1)
		struct IndexEntry
		{
			uint32_t fileNameOffset;
			uint32_t dataBlockOffset;
			int32_t fileSize;
			CompressionType compressionType;
		};

		struct SectionHeader
		{
			std::array<char, 4> tag;
			uint32_t length : 31;
			uint32_t padding : 1; // 0 = 4-byte boundary padding, 1 = 2-byte boundary padding
		};
#pragma pack(pop)

		struct CreateVolumeInfo
		{
			std::vector<IndexEntry> indexEntries;
			std::vector<HANDLE> fileHandles;
			std::vector<std::string> filesToPack;
			std::vector<std::string> internalNames;
			uint32_t stringTableLength;
			uint32_t indexTableLength;
			uint32_t paddedStringTableLength;
			uint32_t paddedIndexTableLength;

			std::size_t fileCount() const
			{
				return filesToPack.size();
			}
		};

		uint32_t ReadTag(const std::array<char, 4>& tagName);
		void WriteTag(StreamWriter& volWriter, uint32_t length, const char *tagText);
		void CopyFileIntoVolume(StreamWriter& volWriter, HANDLE inputFile, int32_t size);
		void ReadVolHeader();
		void ReadStringTable();
		void ReadPackedFileCount();
		void WriteVolume(const std::string& fileName, const CreateVolumeInfo& volInfo);
		void WriteFiles(StreamWriter& volWriter, const CreateVolumeInfo &volInfo);
		void WriteHeader(StreamWriter& volWriter, const CreateVolumeInfo &volInfo);
		bool PrepareHeader(CreateVolumeInfo &volInfo);
		bool OpenAllInputFiles(CreateVolumeInfo &volInfo);
		void CleanUpVolumeCreate(CreateVolumeInfo &volInfo);
		SectionHeader GetSectionHeader(int index);

		std::unique_ptr<SeekableStreamReader> archiveFileReader;
		uint32_t m_NumberOfIndexEntries;
		//char *m_StringTable;
		std::vector<std::string> m_StringTable;
		uint32_t m_HeaderLength;
		uint32_t m_StringTableLength;
		uint32_t m_IndexTableLength;
		std::vector<IndexEntry> m_IndexEntries;
	};
}
