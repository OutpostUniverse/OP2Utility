#pragma once

#include "BidirectionalWriter.h"
#include <string>
#include <fstream>
#include <cstddef>
#include <cstdint>

namespace Stream
{
	class FileWriter : public BidirectionalWriter
	{
	public:
		// Open mode bit flags
		enum OpenMode {
			CanOpenExisting = 0b0000'0001,
			CanOpenNew = 0b0000'0010,
			Truncate = 0b0000'0100,
			Append = 0b0000'1000,

			Default = CanOpenExisting | CanOpenNew | Truncate,
		};

		// Note: A Time-of-check to time-of-use race condition may exist when
		// certain OpenMode flags are cleared:
		//   CanOpenExisting
		//   CanOpenNew
		// If both flags are specified, no race condition can occur
		FileWriter(const std::string& filename, OpenMode openMode = OpenMode::Default);
		FileWriter(FileWriter&& fileWriter);
		~FileWriter() override;

		// SeekableWriter methods
		uint64_t Length() override;
		uint64_t Position() override;

		void Seek(uint64_t position) override;
		void SeekForward(uint64_t offset) override;
		void SeekBackward(uint64_t offset) override;

		inline const std::string& GetFilename() const {
			return filename;
		}

	protected:
		void WriteImplementation(const void* buffer, std::size_t size) override;

		static std::ios_base::openmode TranslateFlags(const std::string& filename, OpenMode openMode);

	private:
		const std::string filename;
		std::ofstream file;
	};
}
