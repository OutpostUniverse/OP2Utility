#pragma once

#include "HuffLZ.h"
#include "ArchiveFile.h"
#include "CompressionType.h"
#include "../Tag.h"
#include "../Stream/FileWriter.h"
#include "../Stream/FileReader.h"
#include <cstddef>
#include <string>
#include <vector>
#include <array>
#include <memory>

namespace Archive
{
	class VolFile : public ArchiveFile
	{
	public:
		VolFile(const std::string& filename);
		~VolFile();

		// Internal file status
		//std::size_t GetIndex(const std::string& name) override;
		std::string GetName(std::size_t index) override;
		CompressionType GetCompressionCode(std::size_t index);
		uint32_t GetSize(std::size_t index) override;

		// Extraction
		void ExtractFile(std::size_t index, const std::string& pathOut) override;

		// Opens a stream containing a packed file
		std::unique_ptr<Stream::BidirectionalReader> OpenStream(std::size_t index) override;

		// Create a new archive with the files specified in filesToPack
		static void CreateArchive(const std::string& volumeFilename, std::vector<std::string> filesToPack);

	private:
		int GetFileOffset(std::size_t index);
		int GetFilenameOffset(std::size_t index);

		void ExtractFileUncompressed(std::size_t index, const std::string& filename);
		void ExtractFileLzh(std::size_t index, const std::string& filename);

#pragma pack(push, 1)
		struct IndexEntry
		{
			uint32_t filenameOffset;
			uint32_t dataBlockOffset;
			int32_t fileSize;
			CompressionType compressionType;
		};

		static_assert(14 == sizeof(IndexEntry), "VolFile::IndexEntry is an unexpected size");

		// Specify boundary padding for a volume file section
		enum class VolPadding
		{
			TwoByte = 0,
			FourByte = 1
		};

		struct SectionHeader
		{
			SectionHeader();
			SectionHeader(Tag tag, uint32_t length, VolPadding padding = VolPadding::FourByte);

			Tag tag;
			uint32_t length : 31;
			VolPadding padding : 1;
		};

		static_assert(8 == sizeof(SectionHeader), "VolFile::SectionHeader is an unexpected size");

#pragma pack(pop)

		struct CreateVolumeInfo
		{
			std::vector<IndexEntry> indexEntries;
			std::vector<std::unique_ptr<Stream::BidirectionalReader>> fileStreamReaders;
			std::vector<std::string> filesToPack;
			std::vector<std::string> names;
			uint32_t stringTableLength;
			uint32_t indexTableLength;
			uint32_t paddedStringTableLength;
			uint32_t paddedIndexTableLength;

			std::size_t fileCount() const
			{
				return filesToPack.size();
			}
		};

		uint32_t ReadTag(Tag tagName);
		void ReadVolHeader();
		void ReadStringTable();
		void CountValidEntries();
		SectionHeader GetSectionHeader(std::size_t index);

		static void WriteVolume(const std::string& filename, CreateVolumeInfo& volInfo);
		static void WriteFiles(Stream::Writer& volWriter, CreateVolumeInfo &volInfo);
		static void WriteHeader(Stream::Writer& volWriter, const CreateVolumeInfo &volInfo);
		static void PrepareHeader(CreateVolumeInfo &volInfo, const std::string& volumeFilename);
		static void OpenAllInputFiles(CreateVolumeInfo &volInfo, const std::string& volumeFilename);

		Stream::FileReader archiveFileReader;
		uint32_t m_IndexEntryCount;
		std::vector<std::string> m_StringTable;
		uint32_t m_HeaderLength;
		uint32_t m_StringTableLength;
		uint32_t m_IndexTableLength;
		std::vector<IndexEntry> m_IndexEntries;
	};
}
