#include "./filtered_string_view.h"

#include <catch2/catch.hpp>
#include <string>

// 2.3 检查默认谓词函数对所有字符是否都返回 true
TEST_CASE("Default predicate returns true for all chars") {
	for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); c++) {
		assert(fsv::filtered_string_view::default_predicate(c));
	}
}

// 2.4.1 检查默认构造函数是否创建了一个空的视图
TEST_CASE("Default constructor creates an empty view") {
	auto sv = fsv::filtered_string_view{};
	REQUIRE(sv.size() == 0);
}

// 2.4.2 检查从 std::string 隐式转换为 fsv::filtered_string_view 的行为
TEST_CASE("Implicit String Constructor") {
	auto s = std::string{"cat"};
	auto sv = fsv::filtered_string_view{s}; // 隐式转换

	REQUIRE(sv.size() == s.size());
	REQUIRE(std::string(sv.data(), sv.size()) == s);
}

// 2.4.3
TEST_CASE("String Constructor with Predicate") {
	auto s = std::string{"cat"};
	auto pred = [](const char& c) { return c == 'a'; };
	auto sv = fsv::filtered_string_view{s, pred}; // 使用谓词的构造函数

	REQUIRE(sv.size() == 1); // 只有一个字符 'a' 符合谓词条件
	REQUIRE(std::string(sv.data(), sv.size()) == "a"); // 验证数据内容
}
