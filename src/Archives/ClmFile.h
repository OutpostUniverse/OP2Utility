#pragma once

#include "WaveFile.h"
#include "ArchiveFile.h"
#include "../Streams/FileReader.h"
#include "../Streams/FileWriter.h"
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <memory>

namespace Archives
{
	class ClmFile : public ArchiveFile
	{
	public:
		ClmFile(const std::string& filename);
		virtual ~ClmFile();

		std::string GetName(std::size_t index) override;
		uint32_t GetSize(std::size_t index) override;
		void ExtractFile(std::size_t index, const std::string& pathOut) override;

		// Opens a stream containing packed audio PCM data
		std::unique_ptr<Stream::SeekableReader> OpenStream(std::size_t index) override;

		void Repack() override;

		// Create a new archive with the files specified in filesToPack
		static void CreateArchive(const std::string& archiveFilename, std::vector<std::string> filesToPack);

	private:
#pragma pack(push, 1)
		struct ClmHeader
		{
			ClmHeader() {};
			ClmHeader(WaveFormatEx waveFormatEx, uint32_t packFileCount);

			std::array<char, 32> fileVersion;
			WaveFormatEx waveFormat;
			std::array<char, 6> unknown;
			uint32_t packedFilesCount;

			bool CheckFileVersion() const;
			bool CheckUnknown() const;

			void VerifyFileVersion() const; // Exception raised if invalid version
			void VerifyUnknown() const; // Exception raised if invalid version
		};

		struct IndexEntry
		{
			std::array<char, 8> filename;
			uint32_t dataOffset;
			uint32_t dataLength;

			std::string GetFilename() const;
		};
#pragma pack(pop)

		// Private functions for reading archive
		void ReadHeader();

		// Private functions for packing files
		static void ReadAllWaveHeaders(std::vector<std::unique_ptr<Stream::FileReader>>& filesToPackReaders, std::vector<WaveFormatEx>& waveFormats, std::vector<IndexEntry>& indexEntries);
		static uint32_t FindChunk(std::array<char, 4> chunkTag, Stream::SeekableReader& seekableStreamReader);
		static void CompareWaveFormats(const std::vector<WaveFormatEx>& waveFormatsconst, const std::vector<std::string>& filesToPack);
		static void WriteArchive(const std::string& archiveFilename, const std::vector<std::unique_ptr<Stream::FileReader>>& filesToPackReaders,
			std::vector<IndexEntry>& indexEntries, const std::vector<std::string>& names, const WaveFormatEx& waveFormat);
		static void PrepareIndex(int headerSize, const std::vector<std::string>& names, std::vector<IndexEntry>& indexEntries);
		static std::vector<std::string> StripFilenameExtensions(std::vector<std::string> paths);
		static WaveFormatEx PrepareWaveFormat(const std::vector<WaveFormatEx>& waveFormats);

		Stream::FileReader clmFileReader;
		ClmHeader clmHeader;
		std::vector<IndexEntry> indexEntries;
	};
}
