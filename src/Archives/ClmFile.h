#pragma once

#include "WaveFile.h"
#include "ArchiveFile.h"
#include "../Streams/FileStreamReader.h"
#include "../Streams/FileStreamWriter.h"
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

		std::string GetInternalName(std::size_t index);
		void ExtractFile(std::size_t index, const std::string& pathOut);

		// Opens a stream containing packed audio PCM data
		std::unique_ptr<SeekableStreamReader> OpenStream(std::size_t index);

		uint32_t GetInternalItemSize(std::size_t index);

		void Repack();

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

		void InitializeWaveHeader(WaveHeader& headerOut, std::size_t index);

		// Private functions for packing files
		static void ReadAllWaveHeaders(std::vector<std::unique_ptr<FileStreamReader>>& filesToPackReaders, std::vector<WaveFormatEx>& waveFormats, std::vector<IndexEntry>& indexEntries);
		static uint32_t FindChunk(std::array<char, 4> chunkTag, SeekableStreamReader& seekableStreamReader);
		static void CompareWaveFormats(const std::vector<WaveFormatEx>& waveFormatsconst, const std::vector<std::string>& filesToPack);
		static void WriteArchive(const std::string& archiveFilename, const std::vector<std::unique_ptr<FileStreamReader>>& filesToPackReaders,
			std::vector<IndexEntry>& indexEntries, const std::vector<std::string>& internalNames, const WaveFormatEx& waveFormat);
		static void PrepareIndex(int headerSize, const std::vector<std::string>& internalNames, std::vector<IndexEntry>& indexEntries);
		static std::vector<std::string> StripFilenameExtensions(std::vector<std::string> paths);
		static WaveFormatEx PrepareWaveFormat(const std::vector<WaveFormatEx>& waveFormats);

		FileStreamReader clmFileReader;
		ClmHeader clmHeader;
		std::vector<IndexEntry> indexEntries;
	};
}
