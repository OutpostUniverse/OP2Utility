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


using Tag = std::array<char, 4>;

// Factory method to construct a Tag from a string literal
constexpr Tag MakeTag(const char (&tagText)[5]) {
	return RemoveLastElement(tagText);
}

// Helper method to allow easily printing a Tag's text
std::ostream& operator << (std::ostream& out, const Tag tag) {
	return out.write(tag.data(), tag.size());
}
