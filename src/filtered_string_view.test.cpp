#include "./filtered_string_view.h"

#include <catch2/catch.hpp>
#include <functional>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

// 2.3 Check whether the default predicate function returns true for all characters
TEST_CASE("Default predicate returns true for all chars") {
	for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
		assert(fsv::filtered_string_view::default_predicate(c));
	}
}

// 2.4.1 Check if the default constructor creates an empty view
TEST_CASE("Default constructor creates an empty view") {
	auto sv = fsv::filtered_string_view{};
	REQUIRE(static_cast<int>(sv.size()) == 0);
}

// 2.4.2 Check the behavior of implicit conversion from std::string to fsv::filtered_string_view
TEST_CASE("Implicit String Constructor") {
	auto s = std::string{"cat"};
	auto sv = fsv::filtered_string_view{s}; // Implicit Conversion

	REQUIRE(sv.size() == s.size());
	REQUIRE(static_cast<std::string>(sv) == s);
}

// 2.4.3 Check whether the filtered content can be displayed
TEST_CASE("String Constructor with Predicate") {
	auto s = std::string{"cat"};
	auto pred = [](const char& c) { return c == 'a'; };
	auto sv = fsv::filtered_string_view{s, pred};

	REQUIRE(sv.size() == 1); // Only one character 'a' meets the predicate condition
}

TEST_CASE("String Constructor with Predicate that matches no characters") {
	auto s = std::string{"cat"};
	auto pred = [](const char& c) { return c == 'z'; };
	auto sv = fsv::filtered_string_view{s, pred};

	REQUIRE(static_cast<int>(sv.size()) == 0); // No characters meet the predicate condition
}

TEST_CASE("String Constructor with Predicate that matches all characters") {
	auto s = std::string{"cat"};
	auto pred = [](const char& c) {
		(void)c;
		return true;
	};
	auto sv = fsv::filtered_string_view{s, pred};

	REQUIRE(sv.size() == 3); // All characters meet the predicate condition
}

TEST_CASE("String Constructor with Predicate that matches characters intermittently") {
	auto s = std::string{"banana"};
	auto pred = [](const char& c) { return c == 'a'; }; // Only 'a' matches the condition
	auto sv = fsv::filtered_string_view{s, pred};

	REQUIRE(sv.size() == 3); // There are three 'a's that meet the condition
	REQUIRE(static_cast<std::string>(sv) == "aaa");
}

// 2.4.4 Implicit Null-Terminated String Constructor
TEST_CASE("Filtered_string_view constructed from C-style string") {
	auto sv = fsv::filtered_string_view{"cat"};
	REQUIRE(sv.size() == 3);
	REQUIRE(static_cast<std::string>(sv) == "cat");
}

// 2.4.5 Null-Terminated String with Predicate Constructor
TEST_CASE("Filtered_string_view constructed from C-style string with predicate") {
	auto pred = [](const char& c) { return c == 'a'; };
	auto sv = fsv::filtered_string_view{"cat", pred};

	REQUIRE(sv.size() == 1); // Only 'a' matches the condition
	REQUIRE(static_cast<std::string>(sv) == "a");
}

TEST_CASE("Filtered_string_view constructed from C-style string with predicate - more chars") {
	const char* test_str = "example";
	auto pred = [](const char& c) { return c == 'a' || c == 'e'; };
	fsv::filtered_string_view sv(test_str, pred);

	REQUIRE(sv.size() == 3); // 'e', 'a', 'e' A total of three characters meet the predicate condition
	REQUIRE(static_cast<std::string>(sv) == "eae");
}

// 2.4.6 Copy and Move Constructor
TEST_CASE("Copy constructor shares the same data") {
	auto sv1 = fsv::filtered_string_view{"bulldog"};
	const auto copy = sv1; // Use = to copy

	REQUIRE(copy.data() == sv1.data()); // Check if the copied object shares the same data pointer as the original
	                                    // object
}

TEST_CASE("Move constructor transfers ownership correctly") {
	auto sv1 = fsv::filtered_string_view{"bulldog"};
	const auto move = std::move(sv1); // Use std::move to move

	REQUIRE(sv1.data() == nullptr);
	REQUIRE(static_cast<int>(sv1.size()) == 0);
}

// 2.5.2 Overloading of =
TEST_CASE("filtered_string_view operator==") {
	auto pred = [](const char& c) { return c == '4' || c == '2'; };
	auto fsv1 = fsv::filtered_string_view{"42 bro", pred};
	auto fsv2 = fsv::filtered_string_view{};
	fsv2 = fsv1;

	REQUIRE(fsv1 == fsv2);
	REQUIRE(static_cast<std::string>(fsv1) == "42");
	REQUIRE(static_cast<std::string>(fsv2) == "42");
}

// Overloading of =
TEST_CASE("Move assignment transfers state correctly", "[move_assignment]") {
	auto pred = [](const char& c) { return c == '8' || c == '9'; };
	fsv::filtered_string_view fsv1{"89 baby", pred};
	auto fsv2 = fsv::filtered_string_view{};
	fsv2 = std::move(fsv1);

	// Check whether fs2 has correctly received the resource of fs1
	REQUIRE((static_cast<int>(fsv1.size()) == 0 and fsv1.data() == nullptr));
	REQUIRE((static_cast<int>(fsv2.size()) == 2 and fsv2.data() != nullptr));
	REQUIRE(static_cast<std::string>(fsv2) == "89");
}

// 2.5.4 Overloading of []
TEST_CASE("filtered_string_view subscript access") {
	auto pred = [](const char& c) { return c == '9' || c == '0' || c == ' '; };
	auto fsv1 = fsv::filtered_string_view{"only 90s kids understand", pred};

	REQUIRE(fsv1[2] == '0');
	REQUIRE(static_cast<std::string>(fsv1) == " 90  ");
}

// 2.5.5 Type conversion operators, allowing fsv to be explicitly converted to std::string
TEST_CASE("String Type Conversion") {
	auto sv = fsv::filtered_string_view("vizsla");
	auto s = static_cast<std::string>(sv);

	std::cout << std::boolalpha << (sv.data() == s.data()) << std::endl;
	REQUIRE(sv.data() != s.data()); // sv.data() and s.data() should point to different memory locations
	REQUIRE(static_cast<std::string>(s) == "vizsla");
}

TEST_CASE("String Type Conversion2") {
	auto s = std::string{"cat"};
	auto pred = [](const char& c) { return c == 'a'; };
	auto sv = fsv::filtered_string_view{s, pred};

	auto str = static_cast<std::string>(sv); // Convert filtered_string_view to std::string
	REQUIRE(str.data() != sv.data()); // str.data() and sv.data() should point to different memory locations
	REQUIRE(str == "a");
}

// 2.6.1 Return a character from the fsv according to the index
TEST_CASE("filtered_string_view valid access") {
	auto vowels = std::set<char>{'a', 'A', 'e', 'E', 'i', 'I', 'o', 'O', 'u', 'U'};
	auto is_vowel = [&vowels](const char& c) { return vowels.contains(c); };
	auto sv = fsv::filtered_string_view{"Malamute", is_vowel};
	auto str = static_cast<std::string>(sv);

	REQUIRE(str == "aaue");
	REQUIRE(sv.at(0) == 'a');
}

TEST_CASE("filtered_string_view invalid access") {
	fsv::filtered_string_view sv("");
	try {
		sv.at(0);
	} catch (const std::domain_error& e) {
		std::cout << e.what();
		REQUIRE(std::string(e.what()) == "filtered_string_view::at(0): invalid index");
	}

	REQUIRE_THROWS_AS(sv.at(0), std::domain_error);
}

// 2.6.2 Return the size of the fsv
TEST_CASE("filtered_string_view size of filtered strings") {
	auto sv = fsv::filtered_string_view{"Maltese"};

	REQUIRE(sv.size() == 7);
	REQUIRE(static_cast<std::string>(sv) == "Maltese");
}

TEST_CASE("filtered_string_view size of filtered strings2") {
	auto sv = fsv::filtered_string_view{"Toy Poodle", [](const char& c) { return c == 'o'; }};

	REQUIRE(sv.size() == 3);
	REQUIRE(static_cast<std::string>(sv) == "ooo");
}

// 2.6.3 Return whether the fsv is empty
TEST_CASE("Empty check for non-empty filtered string view1") {
	auto sv = fsv::filtered_string_view("Australian Shepherd");
	auto empty_sv = fsv::filtered_string_view{};
	std::cout << std::boolalpha << sv.empty() << ' ' << empty_sv.empty();

	REQUIRE_FALSE(sv.empty());
	REQUIRE(empty_sv.empty());
}

TEST_CASE("Empty check for empty filtered string view2") {
	auto sv = fsv::filtered_string_view("Border Collie", [](const char& c) { return c == 'z'; });
	std::cout << std::boolalpha << sv.empty();

	REQUIRE(sv.empty()); // Should return true, since no characters match the predicate and the view is empty
}

// 2.6.4 Return the pointer to the underlying data
TEST_CASE("Data ignores filtering and outputs the entire string") {
	auto s = "Sum 42";
	auto sv = fsv::filtered_string_view{s, [](const char& /* c */) { return false; }};

	REQUIRE(sv.empty()); // sv is empty, but the underlying data of sv remains unchanged
	for (auto ptr = sv.data(); *ptr; ++ptr) {
		std::cout << *ptr;
	}

	std::string output;
	auto ptr = sv.data();
	if (ptr) { // Make sure ptr is not a null pointer
		while (*ptr) {
			output += *ptr++;
		}
	}

	REQUIRE(output == "Sum 42");
}

// 2.6.5 Return the predicate used for filtering
TEST_CASE("Access and call predicate function") {
	const auto print_and_return_true = [](const char&) {
		std::cout << "hi!";
		return true;
	};

	const auto s = fsv::filtered_string_view{"doggo", print_and_return_true};

	const auto& predicate = s.predicate();
	predicate(char{});
}

// 2.7.1. Overloading of ==, compares two fsv lexicographically for equality
TEST_CASE("filtered_string_view equality and inequality") {
	auto lo = fsv::filtered_string_view{"aaa"};
	auto hi = fsv::filtered_string_view{"zzz"};

	REQUIRE_FALSE(lo == hi);
	REQUIRE(lo != hi);
}

// 2.7.2 Overloading of <=>
TEST_CASE("Spaceship operator for filtered_string_view") {
	auto lo = fsv::filtered_string_view{"aaa"};
	auto hi = fsv::filtered_string_view{"zzz"};

	std::cout << std::boolalpha << (lo < hi) << ' ' << (lo <= hi) << ' ' << (lo > hi) << ' ' << (lo >= hi) << ' '
	          << (lo <=> hi == std::strong_ordering::less);

	REQUIRE((lo < hi) == true);
	REQUIRE((lo <= hi) == true);
	REQUIRE((lo > hi) == false);
	REQUIRE((lo >= hi) == false);
	REQUIRE((lo == hi) == false);
	REQUIRE((lo != hi) == true);
	REQUIRE((lo <=> hi == std::strong_ordering::less));
}

// // 2.7.3 Overloading of <<
TEST_CASE("Output operator for filtered_string_view") {
	fsv::filtered_string_view fsv("c++ > rust > java", [](const char& c) { return c == 'c' || c == '+'; });
	std::cout << fsv;

	std::stringstream ss;
	ss << fsv;
	REQUIRE(ss.str() == "c++");
}

// 2.8.1 compose
TEST_CASE("Compose function combines multiple filters") {
	fsv::filtered_string_view best_languages{"c / c++"};
	auto vf = std::vector<fsv::filter>{[](const char& c) { return c == 'c' || c == '+' || c == '/'; },
	                                   [](const char& c) { return c > ' '; },
	                                   [](const char& /* unused */) { return true; }};

	auto sv = fsv::compose(best_languages, vf);
	std::cout << sv;

	std::stringstream ss;
	ss << sv;
	REQUIRE(ss.str() == "c/c++");
}

TEST_CASE("Compose function with all true filters") {
	fsv::filtered_string_view best_languages{"c / c++"};
	auto vf = std::vector<fsv::filter>{[](const char& /* c */) { return true; },
	                                   [](const char& /* c */) { return true; },
	                                   [](const char& /* c */) { return true; }};

	auto sv = fsv::compose(best_languages, vf);
	std::cout << sv;

	std::stringstream ss;
	ss << sv;
	REQUIRE(ss.str() == "c / c++");
}

// 2.8.2 split
TEST_CASE("Split with mixed case and special characters") {
	auto interest = std::set<char>{'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F', ' ', '/'};
	auto sv = fsv::filtered_string_view{"0xDEADBEEF / 0xdeadbeef",
	                                    [&interest](const char& c) { return interest.contains(c); }};
	auto tok = fsv::filtered_string_view{" / "};
	auto v = fsv::split(sv, tok);

	std::cout << v[0] << " " << v[1];
}

TEST_CASE("Split with single character delimiter and non-empty segments") {
	auto sv = fsv::filtered_string_view{"xax"};
	auto tok = fsv::filtered_string_view{"x"};
	auto v = fsv::split(sv, tok);
	auto expected = std::vector<fsv::filtered_string_view>{"", "a", ""};

	CHECK(v == expected);
}

TEST_CASE("Split with single character delimiter and empty segments") {
	auto sv = fsv::filtered_string_view{"xx"};
	auto tok = fsv::filtered_string_view{"x"};
	auto v = fsv::split(sv, tok);
	auto expected = std::vector<fsv::filtered_string_view>{"", "", ""};

	CHECK(v == expected);
}

TEST_CASE("Split with empty string") {
	auto sv = fsv::filtered_string_view{""};
	auto tok = fsv::filtered_string_view{" "};
	auto v = fsv::split(sv, tok);
	auto expected = std::vector<fsv::filtered_string_view>{""};

	CHECK(v == expected);
}

TEST_CASE("Split with string ending with delimiter") {
	auto sv = fsv::filtered_string_view{"hellox"};
	auto tok = fsv::filtered_string_view{"x"};
	auto v = fsv::split(sv, tok);
	auto expected = std::vector<fsv::filtered_string_view>{"hello", ""};

	CHECK(v == expected);
}

TEST_CASE("Split with string starting with delimiter") {
	auto sv = fsv::filtered_string_view{" xhello"};
	auto tok = fsv::filtered_string_view{"x"};
	auto v = fsv::split(sv, tok);
	auto expected = std::vector<fsv::filtered_string_view>{" ", "hello"};

	CHECK(v == expected);
}

TEST_CASE("Split with no delimiter in string") {
	auto sv = fsv::filtered_string_view{"hello"};
	auto tok = fsv::filtered_string_view{"x"};
	auto v = fsv::split(sv, tok);
	auto expected = std::vector<fsv::filtered_string_view>{"hello"};

	CHECK(v == expected);
}

// 2.8.3 substr
TEST_CASE("Substr function extracts part of the string correctly") {
	fsv::filtered_string_view sv{"Siberian Husky"};
	auto result = fsv::substr(sv, 9); // Extract the substring starting at the 9th character
	std::cout << result;
	std::stringstream ss;
	ss << result;
	REQUIRE(ss.str() == "Husky");
}

TEST_CASE("Substr function with predicate filters and extracts upper case letters") {
	auto is_upper = [](const char& c) { return std::isupper(static_cast<unsigned char>(c)); };
	auto sv = fsv::filtered_string_view{"Sled Dog", is_upper};
	auto result = fsv::substr(sv, 0, 2);
	std::cout << result;

	std::stringstream ss;
	ss << result;
	REQUIRE(ss.str() == "SD");
}

TEST_CASE("Substr function extracts from the beginning of the string") {
	fsv::filtered_string_view sv{"Samoyed"};
	auto result = fsv::substr(sv, 0, 3); // Extract 3 characters starting from the 0th character
	std::cout << result;
	std::stringstream ss;
	ss << result;
	REQUIRE(ss.str() == "Sam");
}

TEST_CASE("Substr function extracts with length exceeding string length") {
	fsv::filtered_string_view sv{"Collie"};
	auto result = fsv::substr(sv, 4, 10); // Extract 10 characters starting from the 4th character
	std::cout << result;
	std::stringstream ss;
	ss << result;
	REQUIRE(ss.str() == "ie");
}

TEST_CASE("Substr function handles empty string") {
	fsv::filtered_string_view sv{""};
	auto result = fsv::substr(sv, 0, 5); // Extract a substring from an empty string
	std::cout << result;
	std::stringstream ss;
	ss << result;
	REQUIRE(ss.str() == "");
}

TEST_CASE("Substr function extracts middle part of the string") {
	fsv::filtered_string_view sv{"Alaskan Malamute"};
	auto result = fsv::substr(sv, 8, 4); // Extract 4 characters starting from the 8th character
	std::cout << result;
	std::stringstream ss;
	ss << result;
	REQUIRE(ss.str() == "Mala");
}

// 2.9 迭代器
TEST_CASE("Default predicate iteration") {
	auto print_via_iterator = [](const fsv::filtered_string_view& sv) {
		std::ostringstream oss;
		std::copy(sv.begin(), sv.end(), std::ostream_iterator<char>(oss, " "));
		return oss.str();
	};

	auto fsv1 = fsv::filtered_string_view{"corgi"};
	auto output = print_via_iterator(fsv1);
	REQUIRE(output == "c o r g i ");
}

TEST_CASE("Predicate which removes lowercase vowels") {
	auto fsv2 = fsv::filtered_string_view{"samoyed", [](const char& c) {
		                                      return !(c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
	                                      }};
	auto it = fsv2.begin();
	std::ostringstream oss;
	oss << *it << *std::next(it) << *std::next(it, 2) << *std::next(it, 3);
	REQUIRE(oss.str() == "smyd");
}

TEST_CASE("Reverse iteration") {
	const std::string str = "tosa";
	const auto s = fsv::filtered_string_view{str};
	auto it = s.end();
	std::ostringstream oss;
	oss << *std::prev(it) << *std::prev(it, 2);
	REQUIRE(oss.str() == "as");
}

// Check if a character is a letter
bool is_alpha(const char& ch) {
	return std::isalpha(static_cast<unsigned char>(ch));
}

TEST_CASE("Prefix increment operator") {
	const char* str = "123abc456";
	std::function<bool(const char&)> predicate = is_alpha;

	auto fsv = fsv::filtered_string_view(str, predicate);
	auto it = fsv.begin();
	REQUIRE(it != fsv.end()); // Make sure the iterator is not at the end
	CHECK(*it == 'a'); // Check the first character that matches the predicate
	++it;
	REQUIRE(it != fsv.end());
	CHECK(*it == 'b'); // Check the second character that matches the predicate

	++it;
	REQUIRE(it != fsv.end());
	CHECK(*it == 'c'); // Check the third character that matches the predicate
}

TEST_CASE("Postfix increment operator") {
	const char* str = "123abc456";
	std::function<bool(const char&)> predicate = is_alpha;

	auto fsv = fsv::filtered_string_view(str, predicate);
	auto it = fsv.begin();

	REQUIRE(it != fsv.end()); // Make sure the iterator is not at the end
	CHECK(*it == 'a'); // Check the first character that matches the predicate

	it++;
	REQUIRE(it != fsv.end());
	CHECK(*it == 'b'); // Check the second character that matches the predicate

	it++;
	REQUIRE(it != fsv.end());
	CHECK(*it == 'c'); // Check the third character that matches the predicate
}

// 2.10 Range
TEST_CASE("Filter string and store in vector") {
	const auto s = fsv::filtered_string_view{"puppy", [](const char& c) { return !(c == 'u' || c == 'y'); }};
	auto v = std::vector<char>{s.begin(), s.end()};

	REQUIRE(v.size() == 3);
	CHECK(v[0] == 'p');
	CHECK(v[1] == 'p');
	CHECK(v[2] == 'p');
}

TEST_CASE("Reverse filter string and store in vector") {
	const auto s = fsv::filtered_string_view{"milo", [](const char& c) { return !(c == 'i' || c == 'o'); }};
	auto v = std::vector<char>{s.rbegin(), s.rend()};

	REQUIRE(v.size() == 2);
	CHECK(v[0] == 'l');
	CHECK(v[1] == 'm');
}

TEST_CASE("Const begin and end") {
	const auto s = fsv::filtered_string_view{"puppy", [](const char& c) { return !(c == 'u' || c == 'y'); }};
	auto v = std::vector<char>{s.cbegin(), s.cend()};

	REQUIRE(v.size() == 3);
	CHECK(v[0] == 'p');
	CHECK(v[1] == 'p');
	CHECK(v[2] == 'p');
}

TEST_CASE("Const reverse begin and end") {
	const auto s = fsv::filtered_string_view{"milo", [](const char& c) { return !(c == 'i' || c == 'o'); }};
	auto v = std::vector<char>{s.crbegin(), s.crend()};

	REQUIRE(v.size() == 2);
	CHECK(v[0] == 'l');
	CHECK(v[1] == 'm');
}

TEST_CASE("Using reverse_iterator") {
	const auto s = fsv::filtered_string_view{"abcdefg", [](const char& c) { return c != 'c'; }};
	fsv::filtered_string_view::reverse_iterator rit = s.rbegin();

	REQUIRE(*rit == 'g');
	++rit;
	REQUIRE(*rit == 'f');
	++rit;
	REQUIRE(*rit == 'e');
	++rit;
	REQUIRE(*rit == 'd');
	++rit;
	REQUIRE(*rit == 'b');
	++rit;
	REQUIRE(*rit == 'a');
}

TEST_CASE("Using const_reverse_iterator") {
	const auto s = fsv::filtered_string_view{"hijklmn", [](const char& c) { return c != 'k'; }};
	fsv::filtered_string_view::const_reverse_iterator crit = s.crbegin();

	REQUIRE(*crit == 'n');
	++crit;
	REQUIRE(*crit == 'm');
	++crit;
	REQUIRE(*crit == 'l');
	++crit;
	REQUIRE(*crit == 'j');
	++crit;
	REQUIRE(*crit == 'i');
	++crit;
	REQUIRE(*crit == 'h');
}