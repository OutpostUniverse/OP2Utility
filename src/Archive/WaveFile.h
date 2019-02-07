#pragma once

#include "../Tag.h"
#include <array>
#include <cstdint>

namespace Archive
{
	constexpr auto tagRIFF = MakeTag("RIFF");
	constexpr auto tagWAVE = MakeTag("WAVE");
	constexpr auto tagFMT_ = MakeTag("fmt ");
	constexpr auto tagDATA = MakeTag("data");

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

	static_assert(18 == sizeof(WaveFormatEx), "WaveFormatEx is an unexpected size");

	struct RiffHeader
	{
		Tag riffTag;
		uint32_t chunkSize;
		Tag waveTag;
	};

	static_assert(12 == sizeof(RiffHeader), "RiffHeader is an unexpected size");

	struct FormatChunk
	{
		Tag fmtTag;
		uint32_t formatSize;
		WaveFormatEx waveFormat;
	};

	static_assert(8 + sizeof(WaveFormatEx) == sizeof(FormatChunk), "FormatChunk is an unexpected size");

	struct ChunkHeader
	{
		Tag formatTag;
		uint32_t length;
	};

	static_assert(8 == sizeof(ChunkHeader), "ChunkHeader is an unexpected size");

	// http://soundfile.sapp.org/doc/WaveFormat/
	struct WaveHeader
	{
		static WaveHeader Create(const WaveFormatEx& waveFormat, uint32_t dataLength);

		RiffHeader riffHeader;
		FormatChunk formatChunk;
		ChunkHeader dataChunk;
	};

	static_assert(sizeof(RiffHeader) + sizeof(FormatChunk) + sizeof(ChunkHeader) == sizeof(WaveHeader), "WaveHeader is an unexpected size");

#pragma pack(pop)
}
