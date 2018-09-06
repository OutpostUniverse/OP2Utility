#pragma once

#include <array>
#include <cstddef>

struct SectionHeader
{
	SectionHeader();
	SectionHeader(std::array<char, 4> tag, uint32_t length);

	std::array<char, 4> tag;

	// Does not include sectionHeader
	uint32_t length;

	void Validate(std::array<char, 4> tagName) const;

	// Includes sectionHeader
	inline std::size_t TotalLength() const { return length + tag.size() * sizeof(decltype(tag)::value_type); };
};
