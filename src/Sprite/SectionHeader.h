#pragma once

#include "../Tag.h"
#include <array>
#include <cstddef>

#pragma pack(push, 1) // Make sure structures are byte aligned

struct SectionHeader
{
	SectionHeader();
	SectionHeader(Tag tag, uint32_t length);

	Tag tag;

	// Does not include sectionHeader
	uint32_t length;

	void Validate(Tag tagName) const;

	// Includes sectionHeader
	inline std::size_t TotalLength() const { return length + sizeof(tag); };
};

static_assert(8 == sizeof(SectionHeader), "SectionHeader is an unexpected size");

#pragma pack(pop)
