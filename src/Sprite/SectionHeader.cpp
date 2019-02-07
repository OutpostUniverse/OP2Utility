#include "SectionHeader.h"

SectionHeader::SectionHeader() : length(0) {}
SectionHeader::SectionHeader(Tag tag, uint32_t length) : tag(tag), length(length) {}

void SectionHeader::Validate(Tag tagName) const
{
	if (tag != tagName) {
		throw std::runtime_error("The tag " + std::string(tag.data(), tag.size()) + " should read as " + std::string(tagName.data(), tagName.size()));
	}
}
