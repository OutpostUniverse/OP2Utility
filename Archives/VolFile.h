#pragma once

#include "HuffLZ.h"
#include "ArchiveFile.h"
#include "CompressionType.h"
#include "../Streams/FileStreamWriter.h"
#include "../Streams/FileStreamReader.h"
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
		VolFile(const std::string& fileName);
		~VolFile();

		// Internal file status
		std::string GetInternalFileName(int index);
		int GetInternalFileIndex(const std::string& internalFileName);
		CompressionType GetInternalCompressionCode(int index);
		uint32_t GetInternalFileSize(int index);

		// Extraction
		void ExtractFile(int fileIndex, const std::string& pathOut);
		std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(const std::string& internalFileName);
		std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(int fileIndex);

		// Volume Creation
		void Repack();

		// Create a new archive with the files specified in filesToPack
		static void CreateArchive(const std::string& volumeFileName, std::vector<std::string> filesToPack);

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

		// Specify boundary padding for a volume file section
		enum class VolPadding : uint32_t
		{
			TwoByte = 0,
			FourByte = 1
		};

		struct SectionHeader
		{
			SectionHeader();
			SectionHeader(std::array<char, 4> tag, uint32_t length, VolPadding padding = VolPadding::FourByte);
			std::array<char, 4> tag;
			uint32_t length : 31;
			VolPadding padding : 1;
		};
#pragma pack(pop)

		static_assert(sizeof(SectionHeader) == 8, "SectionHeader not of required size");

		struct CreateVolumeInfo
		{
			std::vector<IndexEntry> indexEntries;
			std::vector<std::unique_ptr<SeekableStreamReader>> fileStreamReaders;
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

		uint32_t ReadTag(std::array<char, 4> tagName);
		void ReadVolHeader();
		void ReadStringTable();
		void ReadPackedFileCount();
		SectionHeader GetSectionHeader(int index);

		static void WriteVolume(const std::string& fileName, CreateVolumeInfo& volInfo);
		static void WriteFiles(StreamWriter& volWriter, CreateVolumeInfo &volInfo);
		static void WriteHeader(StreamWriter& volWriter, const CreateVolumeInfo &volInfo);
		static void PrepareHeader(CreateVolumeInfo &volInfo, const std::string& volumeFileName);
		static void OpenAllInputFiles(CreateVolumeInfo &volInfo, const std::string& volumeFileName);

		FileStreamReader archiveFileReader;
		uint32_t m_NumberOfIndexEntries;
		std::vector<std::string> m_StringTable;
		uint32_t m_HeaderLength;
		uint32_t m_StringTableLength;
		uint32_t m_IndexTableLength;
		std::vector<IndexEntry> m_IndexEntries;
	};
}
