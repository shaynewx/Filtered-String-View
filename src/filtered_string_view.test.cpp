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
	//	REQUIRE(std::string(sv.data(), sv.size()) == s);
}

// 2.4.3 检查是否能显示过滤出的内容
TEST_CASE("String Constructor with Predicate") {
	auto s = std::string{"cat"};
	auto pred = [](const char& c) { return c == 'a'; };
	auto sv = fsv::filtered_string_view{s, pred}; // 使用谓词的构造函数

	REQUIRE(sv.size() == 1); // 只有一个字符 'a' 符合谓词条件
}

TEST_CASE("String Constructor with Predicate that matches no characters") {
	auto s = std::string{"cat"};
	auto pred = [](const char& c) { return c == 'z'; }; // 一个不会匹配任何字符的谓词
	auto sv = fsv::filtered_string_view{s, pred};

	REQUIRE(sv.size() == 0); // 没有字符符合条件
	REQUIRE(sv.data() == nullptr);
}

TEST_CASE("String Constructor with Predicate that matches all characters") {
	auto s = std::string{"cat"};
	auto pred = [](const char& c) {
		(void)c;
		return true;
	}; // 所有字符都符合条件的谓词
	auto sv = fsv::filtered_string_view{s, pred};

	REQUIRE(sv.size() == 3); // 所有字符都应该符合条件
}

TEST_CASE("String Constructor with Predicate that matches characters intermittently") {
	auto s = std::string{"banana"};
	auto pred = [](const char& c) { return c == 'a'; }; // 只有 'a' 符合条件
	auto sv = fsv::filtered_string_view{s, pred};

	REQUIRE(sv.size() == 3); // 有三个 'a' 符合条件
}

// 2.4.4 隐式以空字符结尾的字符串构造函数
TEST_CASE("Filtered_string_view constructed from C-style string") {
	auto sv = fsv::filtered_string_view{"cat\0"};
	REQUIRE(sv.size() == 3); //
}

// 2.4.5 带有谓词的以空字符结尾的字符串构造函数
TEST_CASE("Filtered_string_view constructed from C-style string with predicate") {
	auto pred = [](const char& c) { return c == 'a'; };
	auto sv = fsv::filtered_string_view{"cat", pred};

	REQUIRE(sv.size() == 1); // 只有一个字符 'a' 符合谓词条件
}

TEST_CASE("Filtered_string_view constructed from C-style string with predicate - more chars") {
	const char* test_str = "example";
	auto pred = [](const char& c) { return c == 'a' || c == 'e'; };
	fsv::filtered_string_view sv(test_str, pred);

	REQUIRE(sv.size() == 3); // 'e', 'a', 'e' 一共三个字符符合谓词条件
}

// 2.4.6 拷贝和移动构造函数
TEST_CASE("Copy constructor shares the same data") {
	auto sv1 = fsv::filtered_string_view{"bulldog"};
	const auto copy = sv1; // 使用复制构造函数

	// 检查复制后的对象是否与原始对象共享相同的数据指针
	REQUIRE(copy.data() == sv1.data());
}

TEST_CASE("Move constructor transfers ownership correctly") {
	auto sv1 = fsv::filtered_string_view{"bulldog"};
	const auto move = std::move(sv1); // 使用移动构造函数

	// 检查移动后原始对象的指针是否被设置为 nullptr
	REQUIRE(sv1.data() == nullptr);
}

// 2.5.2 ==运算符的重载
TEST_CASE("filtered_string_view operator==") {
	auto pred = [](const char& c) { return c == '4' || c == '2'; };
	fsv::filtered_string_view sv1("42 bro", pred);
	fsv::filtered_string_view sv2("42 bro", pred);

	REQUIRE(sv1 == sv2);
}

// 2.5.3 =运算符的重载
TEST_CASE("Move assignment transfers state correctly", "[move_assignment]") {
	auto pred = [](const char& c) { return c == '8' || c == '9'; };
	fsv::filtered_string_view fsv1{"89 baby", pred};
	fsv::filtered_string_view fsv2; // 默认构造

	// 执行移动赋值
	fsv2 = std::move(fsv1);

	// 检查fs2是否正确接收了fs1的状态
	assert(fsv1.size() == 0 && fsv1.data() == nullptr);
}
