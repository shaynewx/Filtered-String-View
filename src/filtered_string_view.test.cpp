#include "./filtered_string_view.h"

#include <catch2/catch.hpp>
#include <string>

TEST_CASE("Default predicate returns true for all chars") { // 检查默认谓词函数对所有字符是否都返回 true
	for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
		assert(fsv::filtered_string_view::default_predicate(c));
	}
}

TEST_CASE("Default constructor creates an empty view") { // 检查默认构造函数是否创建了一个空的视图
	auto sv = fsv::filtered_string_view{};
	REQUIRE(sv.size() == 0);
}

TEST_CASE("Implicit String Constructor") { // 检查从 std::string 隐式转换为 fsv::filtered_string_view 的行为
	auto s = std::string{"cat"};
	auto sv = fsv::filtered_string_view{s}; // 隐式转换

	REQUIRE(sv.size() == s.size());
	REQUIRE(std::string(sv.data(), sv.size()) == s);
}