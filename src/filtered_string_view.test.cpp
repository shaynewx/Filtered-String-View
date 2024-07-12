#include "./filtered_string_view.h"

#include <catch2/catch.hpp>
#include <functional>
#include <iostream>
#include <set>
#include <sstream>
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
	REQUIRE(static_cast<int>(sv.size()) == 0);
}

// 2.4.2 检查从 std::string 隐式转换为 fsv::filtered_string_view 的行为
TEST_CASE("Implicit String Constructor") {
	auto s = std::string{"cat"};
	auto sv = fsv::filtered_string_view{s}; // 隐式转换

	REQUIRE(sv.size() == s.size());
	REQUIRE(static_cast<std::string>(sv) == s);
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

	REQUIRE(static_cast<int>(sv.size()) == 0); // 没有字符符合条件
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
	REQUIRE(static_cast<std::string>(sv) == "aaa");
}

// 2.4.4 隐式以空字符结尾的字符串构造函数
TEST_CASE("Filtered_string_view constructed from C-style string") {
	auto sv = fsv::filtered_string_view{"cat"};
	REQUIRE(sv.size() == 3); //
	REQUIRE(static_cast<std::string>(sv) == "cat");
}

// 2.4.5 带有谓词的以空字符结尾的字符串构造函数
TEST_CASE("Filtered_string_view constructed from C-style string with predicate") {
	auto pred = [](const char& c) { return c == 'a'; };
	auto sv = fsv::filtered_string_view{"cat", pred};

	REQUIRE(sv.size() == 1); // 只有一个字符 'a' 符合谓词条件
	REQUIRE(static_cast<std::string>(sv) == "a");
}

TEST_CASE("Filtered_string_view constructed from C-style string with predicate - more chars") {
	const char* test_str = "example";
	auto pred = [](const char& c) { return c == 'a' || c == 'e'; };
	fsv::filtered_string_view sv(test_str, pred);

	REQUIRE(sv.size() == 3); // 'e', 'a', 'e' 一共三个字符符合谓词条件
	REQUIRE(static_cast<std::string>(sv) == "eae");
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
	REQUIRE(static_cast<int>(sv1.size()) == 0);
}

// 2.5.2 复制任务
TEST_CASE("filtered_string_view operator==") {
	auto pred = [](const char& c) { return c == '4' || c == '2'; };
	auto fsv1 = fsv::filtered_string_view{"42 bro", pred};
	auto fsv2 = fsv::filtered_string_view{};
	fsv2 = fsv1;

	REQUIRE(fsv1 == fsv2);
	REQUIRE(static_cast<std::string>(fsv1) == "42");
	REQUIRE(static_cast<std::string>(fsv2) == "42");
}

// 2.5.3 =运算符的重载
TEST_CASE("Move assignment transfers state correctly", "[move_assignment]") {
	auto pred = [](const char& c) { return c == '8' || c == '9'; };
	fsv::filtered_string_view fsv1{"89 baby", pred};
	auto fsv2 = fsv::filtered_string_view{}; // 默认构造

	// 执行移动赋值
	fsv2 = std::move(fsv1);

	// 检查fs2是否正确接收了fs1的状态
	REQUIRE((static_cast<int>(fsv1.size()) == 0 && fsv1.data() == nullptr));
	REQUIRE((static_cast<int>(fsv2.size()) == 2 && fsv2.data() != nullptr));
	REQUIRE(static_cast<std::string>(fsv2) == "89");
}

// 2.5.4 []运算符的重载
TEST_CASE("filtered_string_view subscript access") {
	auto pred = [](const char& c) { return c == '9' || c == '0' || c == ' '; };
	auto fsv1 = fsv::filtered_string_view{"only 90s kids understand", pred};

	REQUIRE(fsv1[2] == '0');
	REQUIRE(static_cast<std::string>(fsv1) == " 90  ");
}

// 2.5.5 类型转换运算符，允许 filtered_string_view 对象显式转换为 std::string
TEST_CASE("String Type Conversion") {
	auto sv = fsv::filtered_string_view("vizsla");
	auto s = static_cast<std::string>(sv);

	std::cout << std::boolalpha << (sv.data() == s.data()) << std::endl;
	REQUIRE(sv.data() != s.data()); // sv.data() 和 s.data() 应指向不同的内存位置
	REQUIRE(static_cast<std::string>(s) == "vizsla");
}

TEST_CASE("String Type Conversion2") {
	auto s = std::string{"cat"};
	auto pred = [](const char& c) { return c == 'a'; };
	auto sv = fsv::filtered_string_view{s, pred}; // 使用谓词的构造函数

	auto str = static_cast<std::string>(sv); // 将 filtered_string_view 转换为 std::string
	REQUIRE(str.data() != sv.data()); // str.data() 和 sv.data() 应指向不同的内存位置
	REQUIRE(str == "a"); // 检查转换后的字符串是否为 "a"
}

// 2.6.1允许根据索引从过滤后的字符串中读取一个字符
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

// 2.6.2 返回已过滤字符串的大小
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

// 2.6.3 返回过滤后的字符串是否为空
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

	REQUIRE(sv.empty()); // 应该返回 true，因为没有字符符合谓词，视图为空
}

// 2.6.4 返回底层指向底层数据的指针
TEST_CASE("Data ignores filtering and outputs the entire string") {
	auto s = "Sum 42";
	auto sv = fsv::filtered_string_view{s, [](const char& /* c */) { return false; }};

	REQUIRE(sv.empty()); // sv为空，但是sv的底层数据不变
	for (auto ptr = sv.data(); *ptr; ++ptr) {
		std::cout << *ptr;
	}

	std::string output;
	auto ptr = sv.data();
	if (ptr) { // 确保 ptr 不是空指针
		while (*ptr) {
			output += *ptr++;
		}
	}

	REQUIRE(output == "Sum 42");
}

// 2.6.5 访问用于进行过滤的谓词
TEST_CASE("Access and call predicate function") {
	// 创建一个打印并返回真值的谓词（这个谓词每次被调用时都打印 "hi!" 并返回 true ，不会过滤任何数据）
	const auto print_and_return_true = [](const char&) {
		std::cout << "hi!";
		return true;
	};

	const auto s = fsv::filtered_string_view{"doggo", print_and_return_true};

	const auto& predicate = s.predicate();
	predicate(char{});
}

// 2.7.1. ==运算符的重载，按字典顺序比较两个filtered_string_view字符串是否相等
TEST_CASE("filtered_string_view equality and inequality") {
	auto lo = fsv::filtered_string_view{"aaa"};
	auto hi = fsv::filtered_string_view{"zzz"};

	REQUIRE_FALSE(lo == hi);
	REQUIRE(lo != hi);
}

// 2.7.2 <=>运算符的重载
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

// // 2.7.3 <<运算符的重载
TEST_CASE("Output operator for filtered_string_view") {
	fsv::filtered_string_view fsv("c++ > rust > java", [](const char& c) { return c == 'c' || c == '+'; });
	std::cout << fsv;

	std::stringstream ss;
	ss << fsv; // 使用自定义的输出运算符
	REQUIRE(ss.str() == "c++"); // 验证输出是否仅包含过滤后的字符
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
	ss << sv; // 使用 operator<< 进行输出
	REQUIRE(ss.str() == "c/c++"); // 验证输出是否正确
}

TEST_CASE("Compose function with all true filters") {
	fsv::filtered_string_view best_languages{"c / c++"};
	auto vf = std::vector<fsv::filter>{[](const char& /* c */) { return true; },
	                                   [](const char& /* c */) { return true; },
	                                   [](const char& /* c */) { return true; }};

	auto sv = fsv::compose(best_languages, vf);
	std::cout << sv;

	std::stringstream ss;
	ss << sv; // 使用 operator<< 进行输出
	REQUIRE(ss.str() == "c / c++"); // 验证输出是否正确
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
	auto result = fsv::substr(sv, 9); // 提取从第9个字符开始的子字符串
	std::cout << result;
	std::stringstream ss;
	ss << result; // 使用 operator<< 进行输出
	REQUIRE(ss.str() == "Husky"); // 验证输出是否为 "Husky"
}

TEST_CASE("Substr function with predicate filters and extracts upper case letters") {
	auto is_upper = [](const char& c) { return std::isupper(static_cast<unsigned char>(c)); };
	auto sv = fsv::filtered_string_view{"Sled Dog", is_upper};
	auto result = fsv::substr(sv, 0, 2);
	std::cout << result;

	std::stringstream ss;
	ss << result; // 使用 operator<< 进行输出
	REQUIRE(ss.str() == "SD"); // 验证输出是否为 "SD"
}

TEST_CASE("Substr function extracts from the beginning of the string") {
	fsv::filtered_string_view sv{"Samoyed"};
	auto result = fsv::substr(sv, 0, 3); // 提取从第0个字符开始的3个字符
	std::cout << result;
	std::stringstream ss;
	ss << result; // 使用 operator<< 进行输出
	REQUIRE(ss.str() == "Sam"); // 验证输出是否为 "Sam"
}

TEST_CASE("Substr function extracts with length exceeding string length") {
	fsv::filtered_string_view sv{"Collie"};
	auto result = fsv::substr(sv, 4, 10); // 提取从第4个字符开始的10个字符
	std::cout << result;
	std::stringstream ss;
	ss << result; // 使用 operator<< 进行输出
	REQUIRE(ss.str() == "ie"); // 验证输出是否为 "ie"
}

TEST_CASE("Substr function handles empty string") {
	fsv::filtered_string_view sv{""};
	auto result = fsv::substr(sv, 0, 5); // 从空字符串提取子字符串
	std::cout << result;
	std::stringstream ss;
	ss << result; // 使用 operator<< 进行输出
	REQUIRE(ss.str() == ""); // 验证输出是否为空字符串
}

TEST_CASE("Substr function extracts middle part of the string") {
	fsv::filtered_string_view sv{"Alaskan Malamute"};
	auto result = fsv::substr(sv, 8, 4); // 提取从第8个字符开始的4个字符
	std::cout << result;
	std::stringstream ss;
	ss << result; // 使用 operator<< 进行输出
	REQUIRE(ss.str() == "Mala"); // 验证输出是否为 "Mala"
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

// 示例谓词：检查字符是否是字母
bool is_alpha(const char& ch) {
	return std::isalpha(static_cast<unsigned char>(ch));
}

TEST_CASE("Prefix increment operator") {
	const char* str = "123abc456";
	std::function<bool(const char&)> predicate = is_alpha;

	auto fsv = fsv::filtered_string_view(str, predicate);
	auto it = fsv.begin();
	REQUIRE(it != fsv.end()); // 确保迭代器不在末尾
	CHECK(*it == 'a'); // 检查第一个符合谓词条件的字符
	++it;
	REQUIRE(it != fsv.end());
	CHECK(*it == 'b'); // 检查第二个符合谓词条件的字符

	++it;
	REQUIRE(it != fsv.end());
	CHECK(*it == 'c'); // 检查第三个符合谓词条件的字符
}

TEST_CASE("Postfix increment operator") {
	const char* str = "123abc456";
	std::function<bool(const char&)> predicate = is_alpha;

	auto fsv = fsv::filtered_string_view(str, predicate);
	auto it = fsv.begin();

	REQUIRE(it != fsv.end()); // 确保迭代器不在末尾
	CHECK(*it == 'a'); // 检查第一个符合谓词条件的字符

	it++;
	REQUIRE(it != fsv.end());
	CHECK(*it == 'b'); // 检查第二个符合谓词条件的字符

	it++;
	REQUIRE(it != fsv.end());
	CHECK(*it == 'c'); // 检查第三个符合谓词条件的字符
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
