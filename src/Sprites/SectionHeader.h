#pragma once

#include <array>
#include <cstddef>

#pragma pack(push, 1) // Make sure structures are byte aligned

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

static_assert(8 == sizeof(SectionHeader), "SectionHeader is an unexpected size");

#pragma pack(pop)
