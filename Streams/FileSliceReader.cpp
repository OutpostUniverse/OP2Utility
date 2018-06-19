#include "FileSliceReader.h"
#include <stdexcept>

FileSliceReader::FileSliceReader(std::string filename, uint64_t startingOffset, uint64_t sliceLength) : 
	fileStreamReader(filename), startingOffset(startingOffset), sliceLength(sliceLength)
{
	if (startingOffset + sliceLength < startingOffset) {
		throw std::runtime_error("The supplied starting offset & length cause the ending offset to wrap past the largest allowed value in the FileSliceReader created on file " + 
			fileStreamReader.GetFilename());
	}

	if (startingOffset + sliceLength > fileStreamReader.Length()) {
		throw std::runtime_error("The ending offset of the FileSliceReader created on " + 
			fileStreamReader.GetFilename() + "is greater than the file's length");
	}

	fileStreamReader.Seek(startingOffset);
}

void FileSliceReader::ReadImplementation(void* buffer, std::size_t size) 
{
	if (fileStreamReader.Position() + size > startingOffset + sliceLength) {
		throw std::runtime_error("File Read request would place the position of the FileSliceReader outside the ending offset of file " + 
			fileStreamReader.GetFilename());
	}

	fileStreamReader.Read(buffer, size);
}

uint64_t FileSliceReader::Length()
{
	return sliceLength;
}
	
uint64_t FileSliceReader::Position() 
{
	return fileStreamReader.Position() - startingOffset;
}

void FileSliceReader::Seek(uint64_t position)
{
	if (position > sliceLength) {
		throw std::runtime_error("An absolute offset of " + std::to_string(position) + 
			" would place the position of the FileSliceReader outside the ending offset of file " + fileStreamReader.GetFilename());
	}

	fileStreamReader.Seek(startingOffset + position);
}

void FileSliceReader::SeekRelative(int64_t offset)
{
	if (Position() + offset < startingOffset ||
		Position() + offset > startingOffset + sliceLength)
	{
		throw std::runtime_error("A relative offset of " + std::to_string(offset) + 
			" would place the position of the FileSliceReader outside the slice bounds of file " + fileStreamReader.GetFilename());
	}

	fileStreamReader.SeekRelative(offset);
}
