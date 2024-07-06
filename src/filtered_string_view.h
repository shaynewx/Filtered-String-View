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

		// 成员函数
		[[nodiscard]] auto data() const -> const char*; // 自定义的成员函数，返回一个指向常量字符串的常量指针
		[[nodiscard]] auto size() const -> size_t; // 自定义的成员函数，返回某个常量数据的大小或长度

		class iter { // class iter 是一个嵌套在 filtered_string_view 内部的迭代器类，用于遍历过滤后的字符串视图
		 public:
			using MEMBER_TYPEDEFS_GO_HERE = void; // 占位符，此处需要添加必要的类型定义
			using difference_type = std::ptrdiff_t;
			using value_type = const char;
			using pointer = const char*;
			using reference = const char&;
			using iterator_category = std::bidirectional_iterator_tag;

			iter(); // 迭代器的默认构造函数，具体实现将在cpp中

			auto operator*() const -> void; // // 重载解引用运算符 （需要修改返回类型）
			auto operator->() const -> void; // // 重载解箭头运算符 （需要修改返回类型）

			auto operator++() -> iter&; // 前置自增运算符，用于迭代器前进
			auto operator++(int) -> iter; // 后置自增运算符，用于迭代器前进
			auto operator--() -> iter&; // 前置自减运算符，用于迭代器后退
			auto operator--(int) -> iter; // 后置自减运算符，用于迭代器后退

			friend auto operator==(const iter&, const iter&) -> bool; // 迭代器友元函数，重载等于比较运算符
			friend auto operator!=(const iter&, const iter&) -> bool; // 迭代器友元函数，重载不等于比较运算符

		 private:
			/* Implementation-specific private members */
			// 添加迭代器的私有成员
		};

	 private:
		const char* pointer_; // 指向底层字符串数据的常量指针
		std::size_t length_; // 底层字符串的长度，这个长度包括所有字符（无论是否被过滤）
		filter predicate_; // 存储传入的谓词函数
	};
} // namespace fsv

#endif // COMP6771_ASS2_FSV_H
