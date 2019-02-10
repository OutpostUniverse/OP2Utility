#pragma once

#include <array>
#include <utility>
#include <iostream>


// Template method implementation details. Not for external use.
namespace Detail {
	template<typename Type, std::size_t N, std::size_t... IndexSequence>
	constexpr std::array<Type, N - 1> RemoveLastElement(const Type (&staticArray)[N], std::index_sequence<IndexSequence...>) {
		return {{staticArray[IndexSequence]...}};
	}
}

// Convert a static array to std::array while removing the last element
// Note: This can be used to strip the null terminator of a char[]
template<typename Type, std::size_t N>
constexpr std::array<Type, N - 1> RemoveLastElement(const Type (&staticArray)[N]) {
	return Detail::RemoveLastElement(staticArray, std::make_index_sequence<N - 1>());
}


class Tag {
public:
	// Allow default construction
	Tag() = default;

	// Allow construction from string literals
	constexpr Tag(const char (&text)[5]) :
		text(RemoveLastElement(text))
	{}

	// Allow construction from other Tag objects
	constexpr Tag(const Tag& other) = default;

	// Equality and inequality comparable
	bool operator ==(const Tag rhs) const {
		return text == rhs.text;
	}
	bool operator !=(const Tag rhs) const {
		return text != rhs.text;
	}

	// Helper method to allow easily converting to std::string
	operator std::string() const {
		return std::string(text.data(), text.size());
	}

	// Helper methods to allow easily concatenating a Tag's text to strings
	friend std::string operator+(const char* string, const Tag tag) {
		return string + static_cast<std::string>(tag);
	}
	friend std::string operator+(const std::string& string, const Tag tag) {
		return string + static_cast<std::string>(tag);
	}

	// Helper method to allow easily printing a Tag's text
	friend std::ostream& operator << (std::ostream& out, const Tag tag) {
		return out.write(tag.text.data(), tag.text.size());
	}

private:
	std::array<char, 4> text;
};


// Factory method to construct a Tag from a string literal
inline constexpr Tag MakeTag(const char (&tagText)[5]) {
	return Tag(tagText);
}
