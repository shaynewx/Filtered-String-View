#include "./filtered_string_view.h"

#include <catch2/catch.hpp>

TEST_CASE("Default predicate returns true for all chars") {
	for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
		assert(fsv::filtered_string_view::default_predicate(c));
	}
}

TEST_CASE("Default constructor creates an empty view") {
	fsv::filtered_string_view sv;
	REQUIRE(sv.size() == 0);
}