#ifndef COMP6771_ASS2_FSV_H
#define COMP6771_ASS2_FSV_H

#include <compare>
#include <functional>
#include <iterator>
#include <optional>
#include <string>

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

		~filtered_string_view() = default; // 2.5 默认析构函数

		// 运算符重载
		auto operator==(const filtered_string_view& other) const -> bool; // 2.5.2 ==运算符的重载
		auto operator=(filtered_string_view&& other) noexcept -> filtered_string_view&; // 2.5.3 =运算符的重载
		auto operator[](int n) const -> const char&; // 2.5.4 []运算符的重载
		explicit operator std::string() const; // 2.5.5 字符串类型转换运算符

		// 成员函数
		[[nodiscard]] auto data() const -> const char*; // 自定义的成员函数，返回一个指向常量字符串的常量指针
		[[nodiscard]] auto size() const -> size_t; // 返回过滤后的字符串视图的长度，即满足谓词条件的字符总数
		auto at(int index) -> const char&; // 2.6.1

	 private:
		const char* pointer_; // 指向原始字符串数据的常量指针
		std::size_t length_; // 存储字符串的原始长度
		filter predicate_; // 过滤条件，即何种字符应该被包含在视图中
	};
} // namespace fsv

#endif // COMP6771_ASS2_FSV_H
