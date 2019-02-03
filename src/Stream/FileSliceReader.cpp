#include "FileSliceReader.h"
#include <stdexcept>
#include <limits>

namespace Stream
{
	FileSliceReader::FileSliceReader(const WrappedStreamType& wrappedStream, uint64_t startingOffset, uint64_t sliceLength) :
		wrappedStream(wrappedStream),
		startingOffset(startingOffset),
		sliceLength(sliceLength)
	{
		Initialize();
	}

	FileSliceReader::FileSliceReader(const FileSliceReader& fileSliceReader) :
		wrappedStream(fileSliceReader.wrappedStream),
		startingOffset(fileSliceReader.startingOffset),
		sliceLength(fileSliceReader.sliceLength)
	{
		Initialize();
	}

	void FileSliceReader::Initialize()
	{
		if (sliceLength > std::numeric_limits<decltype(startingOffset)>::max() - startingOffset) {
			throw std::runtime_error(
				"The stream slice would run past the maximum possible stream length."
				+ IdentifySource()
			);
		}

		if (startingOffset + sliceLength > wrappedStream.Length()) {
			throw std::runtime_error(
				"The stream slice would run past the end of the source stream."
				+ IdentifySource()
			);
		}

		wrappedStream.Seek(startingOffset);
	}

	void FileSliceReader::ReadImplementation(void* buffer, std::size_t size)
	{
		if (wrappedStream.Position() + size > startingOffset + sliceLength) {
			throw std::runtime_error(
				"Stream Read request extends beyond the bounds of the stream slice."
				+ IdentifySource()
			);
		}

		wrappedStream.Read(buffer, size);
	}

	std::size_t FileSliceReader::ReadPartial(void* buffer, std::size_t size) noexcept {
		auto bytesLeft = sliceLength - Position();
		// Note: if !(size < bytesLeft) then bytesLeft fits within a size_t
		std::size_t readSize = (size < bytesLeft) ? size : static_cast<std::size_t>(bytesLeft);

		return wrappedStream.ReadPartial(buffer, readSize);
	}

	uint64_t FileSliceReader::Length()
	{
		return sliceLength;
	}

	uint64_t FileSliceReader::Position()
	{
		return wrappedStream.Position() - startingOffset;
	}

	void FileSliceReader::Seek(uint64_t position)
	{
		if (position > sliceLength) {
			throw std::runtime_error(
				"Seek to absolute offset of " + std::to_string(position) + " is beyond the bounds of the stream slice."
				+ IdentifySource()
			);
		}

		wrappedStream.Seek(startingOffset + position);
	}

	void FileSliceReader::SeekForward(uint64_t offset)
	{
		if (Position() + offset > startingOffset + sliceLength)
		{
			throw std::runtime_error(
				"Seek forward by offset of " + std::to_string(offset) + " is beyond the bounds of the stream slice."
				+ IdentifySource()
			);
		}

		wrappedStream.SeekForward(offset);
	}

	void FileSliceReader::SeekBackward(uint64_t offset)
	{
		if (Position() - offset < startingOffset) {
			throw std::runtime_error(
				"Seek backward by offset of " + std::to_string(offset) + " is beyond the bounds of the stream slice."
				+ IdentifySource()
			);
		}

		wrappedStream.SeekBackward(offset);
	}

	FileSliceReader FileSliceReader::Slice(uint64_t sliceLength)
	{
		FileSliceReader slice = Slice(Position(), sliceLength);

		// Wait until slice is successfully created before seeking forward.
		SeekForward(sliceLength);

		return slice;
	}

	FileSliceReader FileSliceReader::Slice(uint64_t sliceStartPosition, uint64_t sliceLength) const
	{
		if (
			sliceStartPosition + sliceLength > this->sliceLength ||
			sliceLength > std::numeric_limits<decltype(sliceStartPosition)>::max() - sliceStartPosition
		) {
			throw std::runtime_error(
				"Requested stream slice exceeds the bounds of current stream slice."
				+ IdentifySource()
			);
		}

		return FileSliceReader(wrappedStream, this->startingOffset + sliceStartPosition, sliceLength);
	}
}
