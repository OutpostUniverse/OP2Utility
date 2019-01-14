#include "WaveFile.h"

namespace Archive
{
	WaveHeader WaveHeader::Create(const WaveFormatEx& waveFormat, uint32_t dataLength)
	{
		WaveHeader waveHeader;

		waveHeader.riffHeader.riffTag = tagRIFF;
		waveHeader.riffHeader.waveTag = tagWAVE;
		waveHeader.riffHeader.chunkSize = sizeof(waveHeader.riffHeader.waveTag) + sizeof(FormatChunk) + sizeof(ChunkHeader) + dataLength;

		waveHeader.formatChunk.fmtTag = tagFMT_;
		waveHeader.formatChunk.formatSize = sizeof(waveHeader.formatChunk.waveFormat);
		waveHeader.formatChunk.waveFormat = waveFormat;
		waveHeader.formatChunk.waveFormat.cbSize = 0;

		waveHeader.dataChunk.formatTag = tagDATA;
		waveHeader.dataChunk.length = dataLength;

		return waveHeader;
	}
}
