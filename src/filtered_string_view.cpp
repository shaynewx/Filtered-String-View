#include "./filtered_string_view.h"
#include <sstream>

namespace fsv {

	// The default predicate function, which always returns true
	auto filtered_string_view::default_predicate(const char&) -> bool {
		return true;
	}

	// Constructors
	// 2.4.1 Default Constructor
	filtered_string_view::filtered_string_view()
	: pointer_(nullptr)
	, length_(0)
	, predicate_(default_predicate) {}

	// 2.4.2 Implicit String Constructor
	filtered_string_view::filtered_string_view(const std::string& s)
	: pointer_(s.data())
	, length_(s.size())
	, predicate_(default_predicate) {}

	// 2.4.3 String Constructor with Predicate
	filtered_string_view::filtered_string_view(const std::string& s, filter predicate)
	: pointer_(s.data())
	, length_(s.size())
	, predicate_(std::move(predicate)) {}

	// 2.4.4 Implicit Null-Terminated String Constructor
	filtered_string_view::filtered_string_view(const char* str)
	: pointer_(str)
	, length_(std::strlen(str)) // Use strlen to calculate the length of str
	, predicate_(default_predicate) {}

	// 2.4.5 Null-Terminated String with Predicate Constructor
	filtered_string_view::filtered_string_view(const char* str, filter predicate)
	: pointer_(str)
	, length_(std::strlen(str)) // Use strlen to calculate the length of str
	, predicate_(std::move(predicate)) {}

	// 2.4.6 Copy Constructor
	filtered_string_view::filtered_string_view(const filtered_string_view& other)
	: pointer_(other.pointer_)
	, length_(other.length_)
	, predicate_(other.predicate_) {}

	// 2.4.6 Move Constructor
	filtered_string_view::filtered_string_view(filtered_string_view&& other) noexcept
	: pointer_(other.pointer_)
	, length_(other.length_)
	, predicate_(std::move(other.predicate_)) { // Transfer other's resources to the new object
		other.pointer_ = nullptr; // Make sure the pointer no longer points to other
		other.length_ = 0; // Clear the length of other
	}

	// 2.5 Member Operators
	// 2.5.2 Overloading of = for copy asm
	auto filtered_string_view::operator=(const filtered_string_view& other) -> filtered_string_view& {
		if (this != &other) { // Check for self-assignment
			pointer_ = other.pointer_;
			length_ = other.length_;
			predicate_ = other.predicate_;
		}
		return *this;
	}
	// 2.5.3 Overloading of = for move asm
	auto filtered_string_view::operator=(filtered_string_view&& other) noexcept -> filtered_string_view& {
		if (this != &other) {
			pointer_ = other.pointer_;
			length_ = other.length_;
			predicate_ = std::move(other.predicate_); // Transfer other's resources to the new object

			other.pointer_ = nullptr; // Make sure the pointer no longer points to other
			other.length_ = 0; // Clear the length of other
		}
		return *this;
	}

	// Default character for invalid index cases
	const char filtered_string_view::default_char = '\0';

	// 2.5.4 [Overloading of [] to read the character at a specific index position in fsc
	auto filtered_string_view::operator[](int n) const -> const char& {
		const char* temp_ptr = pointer_;
		int count = 0;

		while (*temp_ptr != '\0') { // Loop through the underlying data
			if (predicate_(*temp_ptr)) { // Check whether the current character meets the filtering
				if (count == n) {
					return *temp_ptr; // Return when the nth matching character is found
				}
				count++;
			}
			temp_ptr++;
		}

		return default_char; // Return the default character if index n exceeds the number of qualifying characters
	}

	// 2.5.5 Overloading of std::string, allowing fsv to be explicitly converted to std::string
	filtered_string_view::operator std::string() const {
		std::string result;
		result.reserve(length_); // The maximum length of the result is the length of the original string

		// Use copy_if and back_inserter to add all characters that match the predicate to the result string
		std::copy_if(pointer_, pointer_ + length_, std::back_inserter(result), predicate_);

		return result;
	}

	// 2.6 Member Functions
	// 2.6.1 Return a character from the fsv according to the index
	auto filtered_string_view::at(int index) -> const char& {
		if (index < 0 || index >= static_cast<int>(size())) {
			std::ostringstream oss;
			oss << "filtered_string_view::at(" << index << "): invalid index";
			throw std::domain_error(oss.str());
		}

		const char* temp_ptr = pointer_; // Initialize the pointer to the starting position of the underlying data
		int count = 0; // Record the number of characters that meet the predicate condition

		while (*temp_ptr != '\0') { // Loop through the underlying data
			if (predicate_(*temp_ptr)) { // Check whether the current character meets the filtering
				if (count == index) { // Return when the character matching the criteria is found.
					return *temp_ptr;
				}
				count++;
			}
			temp_ptr++;
		}

		return default_char; // Return the default character if index n exceeds the number of qualifying characters
	}

	// 2.6.2 Return the size of the fsv
	auto filtered_string_view::size() const -> std::size_t {
		return static_cast<std::size_t>(std::count_if(pointer_, pointer_ + length_, predicate_));
	}

	// 2.6.3 Return whether the fsv is empty
	auto filtered_string_view::empty() const -> bool {
		return size() == 0;
	}

	// 2.6.4 Return the pointer to the underlying data
	auto filtered_string_view::data() const -> const char* {
		return pointer_;
	}

	// 2.6.5 Return the predicate used for filtering
	auto filtered_string_view::predicate() const -> const filter& {
		return predicate_;
	}

	// Return the length of the original string
	auto filtered_string_view::original_size() const -> std::size_t {
		return length_;
	}

	// 2.7.1. Overloading of ==, compares two fsv lexicographically for equality
	auto operator==(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool {
		return static_cast<std::string>(lhs) == static_cast<std::string>(rhs);
	}

	// 2.7.2 Overloading of <=>
	auto operator<=>(const filtered_string_view& lhs, const filtered_string_view& rhs) -> std::strong_ordering {
		auto lhs_str = static_cast<std::string>(lhs); // Convert lhs to a string
		auto rhs_str = static_cast<std::string>(rhs); // Convert rhs to a string

		// Use the standard library's comparison functions and C++20's spaceship operator
		return lhs_str.compare(rhs_str) <=> 0;
	}

	// 2.7.3 Overloading of <<
	std::ostream& operator<<(std::ostream& os, const filtered_string_view& fsv) {
		for (std::size_t i = 0; i < fsv.size(); ++i) {
			os << fsv[static_cast<int>(i)]; // Output each filtered character
		}
		return os;
	}

	// 2.8 Non-Member Utility Functions
	// 2.8.1 Compose
	// Accept a fsv object and a vector of predicates and return a new fsv object
	// Combine all filter predicates with and, and return true only when all predicates return true,
	// that is, all filters will filter the same characters
	// As long as one of the predicate functions is false,
	// the new predicate function will short-circuit and immediately return false
	auto compose(const filtered_string_view& fsv, const std::vector<filter>& filts) -> filtered_string_view {
		auto composite_filter = [filts](const char& c) -> bool {
			return std::ranges::all_of(filts, [&](const auto& filt) { return filt(c); });
		};

		return filtered_string_view(fsv.data(), composite_filter);
	}

	// 2.8.2 Split
	// Receive two fsv as parameters, one is fsv and the other is tok, and return a vector
	// Using tok as the delimiter, cut fsv into a series of substrings and return them
	// If tok is at the beginning or end of fsv, the result after splitting may contain an empty fsv
	// If fsv does not contain tok, or fsv is empty, the returned vector contains a copy of fsv
	// fsv::split() can accept an empty delimiter
	auto split(const filtered_string_view& fsv, const filtered_string_view& tok) -> std::vector<filtered_string_view> {
		std::vector<filtered_string_view> result;

		// If fsv is empty or tok is empty, return a copy of fsv
		if (fsv.empty() or tok.empty()) {
			result.push_back(fsv);
			return result;
		}

		const char* start = fsv.data(); // Pointer to the start of fsv
		const char* end = start + fsv.original_size(); // Point to the end of fsv
		const char* current = start; // Pointer used to traverse fsv
		const char* tok_start = tok.data(); //  Pointer to the start of tok
		const std::size_t tok_len = tok.size(); // The length of tok

		while (current < end) {
			// Find the position of the tok
			const char* next = std::search(current, end, tok_start, tok_start + tok_len);

			if (next == end) { // If fsv does not contain tok
				if (current != end) {
					// Create a new null-terminated fsv
					std::size_t len = static_cast<size_t>(end - current);
					char* temp = new char[len + 1];
					std::strncpy(temp, current, len);
					temp[len] = '\0';
					result.emplace_back(temp, fsv.predicate());
				}
				break;
			}
			else {
				// Create a new null-terminated fsv
				std::size_t len = static_cast<size_t>(next - current);
				char* temp = new char[len + 1];
				std::strncpy(temp, current, len);
				temp[len] = '\0';
				result.emplace_back(temp, fsv.predicate());
				current = next + tok_len; // Update current, skipping the currently found tok
			}
		}

		// If fsv ends with tok
		if (current == end && end != start && *(end - tok_len) == *tok_start) {
			result.emplace_back("", fsv.predicate());
		}
		return result;
	}

	// 2.8.3 substr
	// Receives three parameters, fsv, pos and count
	// Return a new filtered_string_view, which presents a "substring" view of the original string
	// This substring starts at pos and has a length of rcount, rcount = count <= 0 ? size() - pos() : count
	// filtered_string_view will be a "" if the length of substring is 0
	auto substr(const filtered_string_view& fsv, int pos, int count) -> filtered_string_view {
		const char* start = fsv.data();
		const char* end = start + fsv.original_size(); // Point to the end of fsv
		const char* current = start;

		// Make sure pos does not exceed the length of the filtered string
		if (pos >= static_cast<int>(fsv.size())) {
			return filtered_string_view("", fsv.predicate());
		}

		// Find the starting position of a substring
		int filtered_pos = 0;
		const char* substr_start = nullptr;

		while (current != end and filtered_pos < pos) {
			if (fsv.predicate()(*current)) {
				++filtered_pos;
			}
			++current;
		}

		// Make sure you find your starting position
		if (current == end) {
			return filtered_string_view("", fsv.predicate());
		}

		substr_start = current;

		// Calculate rcount and find the end position of the substring
		int filtered_count = 0;
		const char* substr_end = nullptr;

		while (current != end && (count <= 0 || filtered_count < count)) {
			if (fsv.predicate()(*current)) {
				++filtered_count;
			}
			++current;
		}

		substr_end = current;

		// Creates a new null-terminated fsv
		std::size_t len = static_cast<size_t>(substr_end - substr_start);
		char* temp = new char[len + 1];
		std::strncpy(temp, substr_start, len);
		temp[len] = '\0';

		return filtered_string_view(temp, fsv.predicate());
	}

	// 2.9 Iterator
	// Constructors of iterator
	filtered_string_view::const_iterator::const_iterator()
	: ptr_(nullptr)
	, predicate_(nullptr) {}

	filtered_string_view::const_iterator::const_iterator(const char* ptr, const filter& predicate)
	: ptr_(ptr)
	, predicate_(&predicate) {
		while (ptr_ && *ptr_ && !(*predicate_)(*ptr_)) {
			++ptr_;
		}
	}

	// Member Operators of iterator
	auto filtered_string_view::const_iterator::operator*() const -> reference {
		return *ptr_;
	}

	auto filtered_string_view::const_iterator::operator++() -> const_iterator& {
		do {
			++ptr_;
		} while (ptr_ && *ptr_ && !(*predicate_)(*ptr_)); // Skip all characters that do not match the predicate
		return *this;
	}

	auto filtered_string_view::const_iterator::operator++(int) -> const_iterator {
		const_iterator tmp = *this;
		++(*this);
		return tmp;
	}

	auto filtered_string_view::const_iterator::operator--() -> const_iterator& {
		do {
			--ptr_;
		} while (ptr_ && *ptr_ && !(*predicate_)(*ptr_)); // Skip all characters that do not match the predicate
		return *this;
	}

	auto filtered_string_view::const_iterator::operator--(int) -> const_iterator {
		const_iterator tmp = *this;
		--(*this);
		return tmp;
	}

	auto filtered_string_view::const_iterator::operator==(const const_iterator& other) const -> bool {
		return ptr_ == other.ptr_;
	}

	auto filtered_string_view::const_iterator::operator!=(const const_iterator& other) const -> bool {
		return ptr_ != other.ptr_;
	}

	// 2.10 begin()、end()、cbegin()、cend()、rbegin()、rend()、crbegin()、crend()
	auto filtered_string_view::begin() const -> const_iterator {
		const char* ptr = pointer_;
		while (ptr != pointer_ + length_ && !predicate_(*ptr)) {
			++ptr;
		}
		return const_iterator(ptr, predicate_);
	}

	auto filtered_string_view::cbegin() const -> const_iterator {
		return begin();
	}

	auto filtered_string_view::end() const -> const_iterator {
		return const_iterator(pointer_ + length_, predicate_);
	}

	auto filtered_string_view::cend() const -> const_iterator {
		return end();
	}

	auto filtered_string_view::rbegin() const -> const_reverse_iterator {
		return const_reverse_iterator(end());
	}

	auto filtered_string_view::crbegin() const -> const_reverse_iterator {
		return rbegin();
	}

	auto filtered_string_view::rend() const -> const_reverse_iterator {
		return const_reverse_iterator(begin());
	}

	auto filtered_string_view::crend() const -> const_reverse_iterator {
		return rend();
	}
} // namespace fsv
