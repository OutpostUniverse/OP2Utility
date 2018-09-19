#include "SectionHeader.h"

SectionHeader::SectionHeader() : length(0) {}
SectionHeader::SectionHeader(std::array<char, 4> tag, uint32_t length) : tag(tag), length(length) {}

void SectionHeader::Validate(std::array<char, 4> tagName) const
{
	if (tag != tagName) {
		throw std::runtime_error("The tag " + std::string(tag.data(), tag.size()) + " should read as " + std::string(tagName.data(), tagName.size()));
	}
}