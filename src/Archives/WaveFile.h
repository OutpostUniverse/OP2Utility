#pragma once

#include <array>
#include <cstdint>

namespace Archives
{
	const std::array<char, 4> tagRIFF{ 'R', 'I', 'F', 'F' };
	const std::array<char, 4> tagWAVE{ 'W', 'A', 'V', 'E' };
	const std::array<char, 4> tagFMT_{ 'f', 'm', 't', ' ' };
	const std::array<char, 4> tagDATA{ 'd', 'a', 't', 'a' };

#pragma pack(push, 1)

	/*
	*  extended waveform format structure used for all non-PCM formats. this
	*  structure is common to all non-PCM formats.
	*  Identical to Windows.h WAVEFORMATEX typedef contained in mmeapi.h
	*/
	struct WaveFormatEx
	{
		uint16_t wFormatTag;         /* format type */
		uint16_t nChannels;          /* number of channels (i.e. mono, stereo...) */
		uint32_t nSamplesPerSec;     /* sample rate */
		uint32_t nAvgBytesPerSec;    /* for buffer estimation */
		uint16_t nBlockAlign;        /* block size of data */
		uint16_t wBitsPerSample;     /* number of bits per sample of mono data */
		uint16_t cbSize;             /* the count in bytes of the size of extra information (after cbSize) */
	};

	struct RiffHeader
	{
		std::array<char, 4> riffTag;
		uint32_t chunkSize;
		std::array<char, 4> waveTag;
	};

	struct FormatChunk
	{
		std::array<char, 4> fmtTag;
		uint32_t formatSize;
		WaveFormatEx waveFormat;
	};

	struct ChunkHeader
	{
		std::array<char, 4> formatTag;
		uint32_t length;
	};

	// http://soundfile.sapp.org/doc/WaveFormat/
	struct WaveHeader
	{
		RiffHeader riffHeader;
		FormatChunk formatChunk;
		ChunkHeader dataChunk;
	};

#pragma pack(pop)
}
