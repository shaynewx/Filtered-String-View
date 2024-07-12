#ifndef COMP6771_ASS2_FSV_H
#define COMP6771_ASS2_FSV_H

#include <algorithm>
#include <compare>
#include <cstring>
#include <functional>
#include <iostream>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

namespace fsv {
	using filter = std::function<bool(const char&)>; // Define the alias

	class filtered_string_view {
	 public:
		static auto default_predicate(const char&) -> bool; // The default predicate function, which always returns true

		// Constructors
		filtered_string_view(); // 2.4.1 Default Constructor
		filtered_string_view(const std::string& s); // 2.4.2 Implicit String Constructor
		filtered_string_view(const std::string& str, filter predicate); // 2.4.3 String Constructor with Predicate
		filtered_string_view(const char* str); // 2.4.4 Implicit Null-Terminated String Constructor
		filtered_string_view(const char* str, filter predicate); // 2.4.5 Null-Terminated String with Predicate
		                                                         // Constructor
		filtered_string_view(const filtered_string_view& other); // 2.4.6 Copy Constructor
		filtered_string_view(filtered_string_view&& other) noexcept; // 2.4.6 Move Constructor
		~filtered_string_view() = default; // 2.5 Destructor

		// 2.5 Member Operators
		auto operator=(const filtered_string_view& other) -> filtered_string_view&; // 2.5.2 Overloading of =
		auto operator=(filtered_string_view&& other) noexcept -> filtered_string_view&; // 2.5.3 Overloading of =

		auto operator[](int n) const -> const char&; // 2.5.4 Overloading of []
		explicit operator std::string() const; // 2.5.5 Overloading of std::string

		// 2.6 Member Functions
		auto original_size() const -> std::size_t; // Return the length of the original string
		auto at(int index) -> const char&; // 2.6.1 Return a character from the fsv according to the index
		auto size() const -> std::size_t; // 2.6.2 Return the size of the fsv
		auto empty() const -> bool; // 2.6.3 Return whether the fsv is empty
		auto data() const -> const char*; // 2.6.4 Return the pointer to the underlying data
		auto predicate() const -> const filter&; // 2.6.5 Return the predicate used for filtering

		// 2.9 Iterator
		class const_iterator {
		 public:
			// Type definition
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = char;
			using difference_type = ptrdiff_t;
			using pointer = void;
			using reference = const char&;

			// Constructors of iterator
			const_iterator();
			const_iterator(const char* ptr, const filter& predicate);

			// Member Operators of iterator
			auto operator*() const -> reference;
			auto operator++() -> const_iterator&;
			auto operator++(int) -> const_iterator;
			auto operator--() -> const_iterator&;
			auto operator--(int) -> const_iterator;
			auto operator==(const const_iterator& other) const -> bool;
			auto operator!=(const const_iterator& other) const -> bool;

		 private:
			const char* ptr_;
			const filter* predicate_;
		};

		using iterator = const_iterator;
		using const_iterator = const_iterator;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		// 2.10 Range
		auto begin() const -> const_iterator;
		auto cbegin() const -> const_iterator;

		auto end() const -> const_iterator;
		auto cend() const -> const_iterator;

		auto rbegin() const -> const_reverse_iterator;
		auto crbegin() const -> const_reverse_iterator;

		auto rend() const -> const_reverse_iterator;
		auto crend() const -> const_reverse_iterator;

	 private:
		const char* pointer_; // A constant pointer to the underlying data
		std::size_t length_; // the length of the string
		filter predicate_; // filter
		static const char default_char; // Default character for invalid index cases
	};

	// 2.7 Operator overloading outside the fsv class
	auto operator==(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool; // 2.7.1. Overloading of
	                                                                                           // ==
	auto operator<=>(const filtered_string_view& lhs,
	                 const filtered_string_view& rhs) -> std::strong_ordering; // 2.7.2 Overloading of <=>
	auto operator<<(std::ostream& os, const filtered_string_view& fsv) -> std::ostream&; // 2.7.3 Overloading of <<

	// 2.8 Non-Member Utility Functions
	auto compose(const filtered_string_view& fsv, const std::vector<filter>& filts) -> filtered_string_view; // 2.8.1
	                                                                                                         // compose
	auto split(const filtered_string_view& fsv,
	           const filtered_string_view& tok) -> std::vector<filtered_string_view>; // 2.8.2 split
	auto substr(const filtered_string_view& fsv, int pos = 0, int count = 0) -> filtered_string_view; // 2.8.3 substr

} // namespace fsv

#endif // COMP6771_ASS2_FSV_H
