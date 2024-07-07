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

		// 成员函数
		[[nodiscard]] auto data() const -> const char*; // 自定义的成员函数，返回一个指向常量字符串的常量指针
		[[nodiscard]] auto size() const -> size_t; // 自定义的成员函数，返回某个常量数据的大小或长度

	 private:
		const char* pointer_; // 指向底层字符串数据的常量指针
		std::size_t length_; // 底层字符串的长度，这个长度包括所有字符（无论是否被过滤）
		filter predicate_; // 存储传入的谓词函数
	};
} // namespace fsv

#endif // COMP6771_ASS2_FSV_H
