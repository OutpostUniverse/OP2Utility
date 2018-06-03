#pragma once

#include <cstdint>

namespace Archives
{
	const uint32_t RIFF = 0x46464952;	// "RIFF" Resource Interchange File Format
	const uint32_t WAVE = 0x45564157;	// "WAVE"
	const uint32_t FMT = 0x20746D66;	// "fmt "
	const uint32_t DATA = 0x61746164;	// "data"

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
		int32_t riffTag;
		uint32_t chunkSize;
		int32_t waveTag;
	};

	struct FormatChunk
	{
		uint32_t fmtTag;
		uint32_t formatSize;
		WaveFormatEx waveFormat;
	};

	struct ChunkHeader
	{
		uint32_t formatTag;
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
