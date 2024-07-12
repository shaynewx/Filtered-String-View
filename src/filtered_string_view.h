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
	using filter = std::function<bool(const char&)>; // 定义别名为filter，是一个接受const
	                                                 // char&参数并返回bool的函数的可调用对象
	class filtered_string_view {
	 public:
		static auto default_predicate(const char&) -> bool; // 默认的 静态成员 谓词函数，总是返回 true
		// 构造函数
		filtered_string_view(); // 2.4.1 默认构造函数
		filtered_string_view(const std::string& s); // 2.4.2 隐式字符串构造函数
		filtered_string_view(const std::string& str, filter predicate); // 2.4.3 带Predicate的字符串构造函数
		filtered_string_view(const char* str); // 2.4.4 隐式以空字符结尾的字符串构造函数
		filtered_string_view(const char* str, filter predicate); // 2.4.5 带有谓词的以空字符结尾的字符串构造函数
		filtered_string_view(const filtered_string_view& other); // 2.4.6 拷贝构造函数
		filtered_string_view(filtered_string_view&& other) noexcept; // 2.4.6 移动构造函数
		filtered_string_view(const char* begin, std::size_t length, filter predicate); // 接受字符开始、长度和谓词
		~filtered_string_view() = default; // 2.5 默认析构函数

		// 运算符重载
		auto operator=(const filtered_string_view& other) -> filtered_string_view&; // 2.5.2 =运算符的重载
		auto operator=(filtered_string_view&& other) noexcept -> filtered_string_view&; // 2.5.3 =运算符的重载

		auto operator[](int n) const -> const char&; // 2.5.4 []运算符的重载
		explicit operator std::string() const; // 2.5.5  字符串类型转换运算符，允许将fsv显式转换为std::string

		// 成员函数
		auto original_size() const -> std::size_t; // 返回初始字符串的长度
		auto at(int index) -> const char&; // 2.6.1 允许根据索引从过滤后的字符串中读取一个字符
		auto size() const -> std::size_t; // 2.6.2 返回已过滤字符串的大小
		auto empty() const -> bool; // 2.6.3 返回过滤后的字符串是否为空
		auto data() const -> const char*; // 2.6.4 返回指向底层数据的指针
		auto predicate() const -> const filter&; // 2.6.5 访问用于进行过滤的谓词

		// 2.9 迭代器 Iterator
		class const_iterator {
		 public:
			// 迭代器内部类型定义
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = char;
			using difference_type = ptrdiff_t;
			using pointer = void;
			using reference = const char&;

			// 迭代器内部构造函数
			const_iterator(); // Default constructor
			const_iterator(const char* ptr, const filter& predicate);

			// 运算符重载
			auto operator*() const -> reference;
			auto operator++() -> const_iterator&;
			auto operator++(int) -> const_iterator;
			auto operator--() -> const_iterator&;
			auto operator--(int) -> const_iterator;
			auto operator==(const const_iterator& other) const -> bool;
			auto operator!=(const const_iterator& other) const -> bool;

		 private:
			const char* ptr_; // 当前字符指针
			const filter* predicate_; // 过滤条件
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
		const char* pointer_; // 指向原始字符串数据的常量指针
		std::size_t length_; // 存储字符串的长度
		filter predicate_; // 过滤条件，即何种字符应该被包含在视图中
		static const char default_char; // 用于无效索引情况的默认字符
	};

	// 2.7 类外部的运算符重载
	auto operator==(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool; // 2.7.1. ==运算符的重载
	auto operator<=>(const filtered_string_view& lhs,
	                 const filtered_string_view& rhs) -> std::strong_ordering; // 2.7.2 <=>运算符的重载
	auto operator<<(std::ostream& os, const filtered_string_view& fsv) -> std::ostream&; // 2.7.3 <<运算符的重载

	// 2.8 类外部的非成员函数
	auto compose(const filtered_string_view& fsv, const std::vector<filter>& filts) -> filtered_string_view; // 2.8.1
	auto split(const filtered_string_view& fsv,
	           const filtered_string_view& tok) -> std::vector<filtered_string_view>; // 2.8.2
	auto substr(const filtered_string_view& fsv, int pos = 0, int count = 0) -> filtered_string_view; // 2.8.3

} // namespace fsv

#endif // COMP6771_ASS2_FSV_H
