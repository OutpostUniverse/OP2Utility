#pragma once

#include "ArchiveFile.h"
#include <windows.h>
#include <string>
#include <vector>
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
		void ExtractFile(int fileIndex, const std::string& pathOut);
		std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(const char *internalFileName);
		std::unique_ptr<SeekableStreamReader> OpenSeekableStreamReader(int fileIndex);

		int GetInternalFileSize(int index);

		bool Repack();
		bool CreateVolume(std::string volumeFileName, std::vector<std::string> filesToPack);

	private:
#pragma pack(push, 1)
		/*
		*  extended waveform format structure used for all non-PCM formats. this
		*  structure is common to all non-PCM formats.
		*  Identical to Windows.h WAVEFORMATEX typedef contained in mmeapi.h
		*/
		struct WaveFormatEx
		{
			unsigned short wFormatTag;         /* format type */
			unsigned short nChannels;          /* number of channels (i.e. mono, stereo...) */
			unsigned long  nSamplesPerSec;     /* sample rate */
			unsigned long  nAvgBytesPerSec;    /* for buffer estimation */
			unsigned short nBlockAlign;        /* block size of data */
			unsigned short wBitsPerSample;     /* number of bits per sample of mono data */
			unsigned short cbSize;             /* the count in bytes of the size of extra information (after cbSize) */
		};

		struct RiffHeader
		{
			int riffTag;
			int chunkSize;
			int waveTag;
		};

		struct FormatChunk
		{
			int fmtTag;
			int formatSize;
			WaveFormatEx waveFormat;
		};

		struct DataChunk
		{
			int dataTag;
			int dataSize;
		};

		struct WaveHeader
		{
			RiffHeader riffHeader;
			FormatChunk formatChunk;
			DataChunk dataChunk;
		};

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
		bool OpenAllInputFiles(std::vector<std::string> filesToPack, HANDLE *fileHandle);
		bool ReadAllWaveHeaders(int numFilesToPack, HANDLE *file, WaveFormatEx *format, IndexEntry *indexEntry);
		int FindChunk(int chunkTag, HANDLE file);
		void CleanUpVolumeCreate(HANDLE outFile, int numFilesToPack, HANDLE *fileHandle, WaveFormatEx *waveFormat, IndexEntry *indexEntry);
		bool CompareWaveFormats(int numFilesToPack, WaveFormatEx *waveFormat);
		bool WriteVolume(HANDLE outFile, int numFilesToPack, HANDLE *fileHandle,
			IndexEntry *entry, std::vector<std::string> internalNames, WaveFormatEx *waveFormat);
		std::vector<std::string> StripFileNameExtensions(std::vector<std::string> paths);
		void InitializeWaveHeader(WaveHeader& headerOut, int fileIndex);

		HANDLE m_FileHandle;
		WaveFormatEx m_WaveFormat;
		char m_Unknown[6];
		IndexEntry *m_IndexEntry;
		char(*m_FileName)[9];
	};
}
