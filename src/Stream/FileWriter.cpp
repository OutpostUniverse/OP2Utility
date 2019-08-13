#include "FileWriter.h"
#include "../XFile.h"
#include <stdexcept>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

namespace Stream
{
	std::ios_base::openmode FileWriter::TranslateFlags(const std::string& filename, OpenMode openMode) {
		// Check for bad flag combinations
		if ((openMode & (OpenMode::CanOpenExisting | OpenMode::CanOpenNew)) == 0) {
			throw std::invalid_argument("Bad open mode for file write. Must allow open to succeed for at least one of existing or new files. Filename: " + filename);
		}
		if ((openMode & OpenMode::Truncate) != 0 && (openMode & OpenMode::Append) != 0) {
			throw std::invalid_argument("Bad open mode for file write. Can not specify file be opened both for append, and to truncate existing contents. Filename: " + filename);
		}

		// File existance checks
		if ((openMode & OpenMode::CanOpenExisting) == 0 && fs::exists(filename)) {
			throw std::runtime_error("Failed to open file for writing. Access to existing file not requested, and file already exists. Filename: " + filename);
		}
		if ((openMode & OpenMode::CanOpenNew) == 0 && !fs::exists(filename)) {
			throw std::runtime_error("Failed to open file for writing. Access to open new file not requested, and file does not currently exist. Filename: " + filename);
		}

		// Translate flags to ios_base flags
		std::ios_base::openmode iosOpenMode = std::ios_base::out | std::ios_base::binary;
		if ((openMode & OpenMode::Truncate) != 0) {
			iosOpenMode |= std::ios_base::trunc;
		}
		if ((openMode & OpenMode::Append) != 0) {
			iosOpenMode |= std::ios_base::ate;
		}
		return iosOpenMode;
	}


	FileWriter::FileWriter(const std::string& filename, OpenMode openMode) :
		filename(filename)
	{
		// Create directory if it does not exist. ofstream will fail if directory does not exist.
		auto directory = XFile::GetDirectory(filename);
		if (!directory.empty() && !XFile::PathExists(directory)) 
		{
			if (!(openMode & OpenMode::CanOpenNew)) {
				throw std::runtime_error("The directory does not exist. The write command was restricted from creating new files or directories.");
			}

			XFile::NewDirectory(directory);
		}

		file = std::ofstream(filename, TranslateFlags(filename, openMode));

		if (!file.is_open()) {
			throw std::runtime_error("File could not be opened. Filename: " + filename);
		}
	}

	FileWriter::FileWriter(FileWriter&& fileWriter) :
		filename(std::move(fileWriter.filename)),
		file(std::move(fileWriter.file))
	{
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

	void FileWriter::Seek(uint64_t position)
	{
		file.seekp(position);
	}

	void FileWriter::SeekForward(uint64_t offset)
	{
		uint64_t newPosition = Position() + offset;
		
		if (newPosition < Position()) {
			throw std::runtime_error("Change in offset puts write position beyond possible bounds of file " + filename);
		}

		file.seekp(newPosition);
	}

	void FileWriter::SeekBackward(uint64_t offset)
	{
		if (offset > Position()) {
			throw std::runtime_error("Change in offset puts write position before beginning bounds of file.");
		}

		file.seekp(Position() - offset);
	}
}
