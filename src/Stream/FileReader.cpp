#include "FileReader.h"
#include "SliceReader.h"
#include <stdexcept>

namespace Stream
{
	// Defers calls to C++ standard library methods
	FileReader::FileReader(std::string filename) :
		filename(filename),
		file(filename, std::ios::in | std::ios::binary)
	{
		Initialize();
	}

	FileReader::FileReader(const FileReader& fileStreamReader) :
		filename(fileStreamReader.filename),
		file(fileStreamReader.filename, std::ios::in | std::ios::binary)
	{
		Initialize();
	}

	void FileReader::Initialize()
	{
		if (!file.is_open()) {
			throw std::runtime_error("Could not open file: " + filename);
		}
	}

	FileReader::~FileReader() {
		file.close();
	}

	void FileReader::ReadImplementation(void* buffer, std::size_t size) {
		file.read(static_cast<char*>(buffer), size);
		// Check stream flags for errors
		if (!file) {
			throw std::runtime_error("Error reading from file");
		}
	}

	std::size_t FileReader::ReadPartial(void* buffer, std::size_t size) noexcept {
		file.read(static_cast<char*>(buffer), size);
		// Note: number of unformatted bytes read, up to size, must fit within a size_t
		return static_cast<std::size_t>(file.gcount());
	}

	uint64_t FileReader::Length() {
		auto currentPosition = file.tellg();  // Record current position
		file.seekg(0, std::ios_base::end);    // Seek to end of file
		auto streamSize = file.tellg();       // Record current position (length of file)
		file.seekg(currentPosition);          // Restore position
		return streamSize;
	}

	uint64_t FileReader::Position() {
		return file.tellg();  // Return the current get pointer
	}

	void FileReader::Seek(uint64_t position) {
		file.seekg(position);
	}

	void FileReader::SeekForward(uint64_t offset) 
	{
		uint64_t newPosition = Position() + offset;

		if (newPosition < Position()) {
			throw std::runtime_error("Change in offset puts read position beyond possible bounds of file " + filename);
		}

		file.seekg(newPosition);
	}

	void FileReader::SeekBackward(uint64_t offset)
	{
		if (offset > Position()) {
			throw std::runtime_error("Change in offset puts read position before beginning bounds of file " + filename);
		}
		
		file.seekg(Position() - offset);
	}

	FileSliceReader FileReader::Slice(uint64_t sliceLength)
	{
		FileSliceReader slice = Slice(Position(), sliceLength);

		// Wait until slice is successfully created before seeking forward.
		SeekForward(sliceLength);

		return slice;
	}

	FileSliceReader FileReader::Slice(uint64_t sliceStartPosition, uint64_t sliceLength) const {
		return FileSliceReader(filename, sliceStartPosition, sliceLength);
	}
}
