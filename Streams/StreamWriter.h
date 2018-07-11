#pragma once

#include <cstddef>
#include <cstdint>

class StreamReader;
class SeekableStreamReader;

class StreamWriter {
public:
	StreamWriter() : streamPassLength(0x00020000) { };
	virtual ~StreamWriter() = default;

	inline void Write(const void* buffer, std::size_t size) {
		WriteImplementation(buffer, size);
	}

	// Writes a length of data from the provided stream starting at the provided stream's current position
	void Write(StreamReader& streamReader, uint64_t length);

	// Writes the remaining length of provided stream from the provided stream's current position
	void Write(SeekableStreamReader& seekableStreamReader);

	// Inline templated convenience methods, to easily write arbitrary data types
	template<typename T> inline void Write(const T& object) {
		WriteImplementation(&object, sizeof(object));
	}

	// StreamPassLength is the buffer size used when copying data from a stream reader into the writer.
	// Changing the pass size may affect performance of copying in streams for writing.
	void SetStreamPassLength(std::size_t streamPassLength);

	inline std::size_t GetStreamPassLength() const { return streamPassLength; }

protected:
	// WriteImplementation is named differently from Write to prevent name hiding of the 
	// Write template helpers in derived classes.
	virtual void WriteImplementation(const void* buffer, std::size_t size) = 0;

private:
	std::size_t streamPassLength;
};
