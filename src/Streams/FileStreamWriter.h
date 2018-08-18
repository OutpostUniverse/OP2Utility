#pragma once

#include "SeekableStreamWriter.h"
#include <string>
#include <fstream>
#include <cstddef>
#include <cstdint>

namespace Stream
{
	class FileWriter : public SeekableWriter
	{
	public:
		FileWriter(const std::string& filename);
		~FileWriter() override;

		// SeekableWriter methods
		uint64_t Length() override;
		uint64_t Position() override;
		void Seek(uint64_t offset) override;
		void SeekRelative(int64_t offset) override;

		inline const std::string& GetFilename() const {
			return filename;
		}

	protected:
		void WriteImplementation(const void* buffer, std::size_t size) override;

	private:
		const std::string filename;
		std::fstream file;
	};
}
