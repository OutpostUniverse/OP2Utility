#include "FileWriter.h"
#include <stdexcept>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

namespace Stream
{
	std::ios_base::openmode FileWriter::TranslateFlags(const std::string& filename, FileWriter::OpenMode openMode) {
		if ((openMode & FileWriter::OpenMode::FailIfExist) != 0 && fs::exists(filename)) {
			throw std::runtime_error("File OpenMode::FailIfExist specified, but file exists: " + filename);
		}
		if ((openMode & FileWriter::OpenMode::FailIfNoExist) != 0 && !fs::exists(filename)) {
			throw std::runtime_error("File OpenMode::FailIfNoExist specified, but file does not exist: " + filename);
		}

		std::ios_base::openmode iosOpenMode = std::ios_base::out | std::ios_base::binary;
		if ((openMode & OpenMode::PreserveContents) == 0) {
			iosOpenMode |= std::ios_base::trunc;
		}
		if ((openMode & OpenMode::Append) != 0) {
			iosOpenMode |= std::ios_base::ate;
		}
		return iosOpenMode;
	}


	FileWriter::FileWriter(const std::string& filename, OpenMode openMode) :
		filename(filename),
		file(filename, TranslateFlags(filename, openMode))
	{
		if (!file.is_open()) {
			throw std::runtime_error("File could not be opened.");
		}
	}

	FileWriter::~FileWriter() {
		file.close();
	}

	void FileWriter::WriteImplementation(const void* buffer, std::size_t size)
	{
		file.write(static_cast<const char*>(buffer), size);
	}

	uint64_t FileWriter::Length()
	{
		auto currentPosition = file.tellp();  // Record current position
		file.seekp(0, std::ios_base::end);    // Seek to end of file
		auto length = file.tellp();   // Record current position (length of file)
		file.seekp(currentPosition);  // Restore position
		return length;
	}

	uint64_t FileWriter::Position()
	{
		return file.tellp();  // Return the current put pointer
	}

	void FileWriter::Seek(uint64_t offset)
	{
		file.seekp(offset);
	}

	void FileWriter::SeekRelative(int64_t offset)
	{
		file.seekp(offset, std::ios_base::cur);
	}
}
