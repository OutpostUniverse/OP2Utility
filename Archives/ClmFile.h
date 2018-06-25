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
		ClmFile(const char *fileName);
		virtual ~ClmFile();

		std::string GetInternalFileName(int index);
		int GetInternalFileIndex(const char *internalFileName);
		void ExtractFile(int fileIndex, const std::string& pathOut);
		std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(const char *internalFileName);
		std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(int fileIndex);

		uint32_t GetInternalFileSize(int index);

		void Repack();
		void CreateArchive(const std::string& archiveFileName, std::vector<std::string> filesToPack);

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
			std::array<char, 8> fileName;
			uint32_t dataOffset;
			uint32_t dataLength;

			std::string GetFileName() const;
		};
#pragma pack(pop)

		// Private functions for reading archive
		void ReadHeader();

		// Private functions for packing files
		void ReadAllWaveHeaders(std::vector<std::unique_ptr<FileStreamReader>>& filesToPackReaders, std::vector<WaveFormatEx>& waveFormats, std::vector<IndexEntry>& indexEntries);
		uint32_t FindChunk(std::array<char, 4> chunkTag, SeekableStreamReader& seekableStreamReader);
		static void CompareWaveFormats(const std::vector<WaveFormatEx>& waveFormats);
		void WriteArchive(const std::string& archiveFileName, const std::vector<std::unique_ptr<FileStreamReader>>& filesToPackReaders,
			std::vector<IndexEntry>& indexEntries, const std::vector<std::string>& internalNames, const WaveFormatEx& waveFormat);
		void PrepareIndex(int headerSize, const std::vector<std::string>& internalNames, std::vector<IndexEntry>& indexEntries);
		std::vector<std::string> StripFileNameExtensions(std::vector<std::string> paths);
		void InitializeWaveHeader(WaveHeader& headerOut, int fileIndex);
		static WaveFormatEx PrepareWaveFormat(const std::vector<WaveFormatEx>& waveFormats);

		FileStreamReader clmFileReader;
		ClmHeader clmHeader;
		std::vector<IndexEntry> indexEntries;
	};
}
