#include "./filtered_string_view.h"
#include <iostream>

namespace fsv {
	// 默认的 静态成员 谓词函数，总是返回 true
	auto filtered_string_view::default_predicate(const char&) -> bool {
		return true;
	}

	// 构造函数的实现
	filtered_string_view::filtered_string_view() // 2.4.1 默认构造函数
	: pointer_(nullptr)
	, length_(0)
	, predicate_(default_predicate) {}

	filtered_string_view::filtered_string_view(const std::string& s) // 2.4.2隐式字符串构造函数
	: pointer_(s.data())
	, length_(s.size())
	, predicate_(default_predicate) {}

	// 成员函数的实现
	auto filtered_string_view::size() const -> size_t {
		return length_;
	}

	auto filtered_string_view::data() const -> const char* {
		return pointer_;
	}
} // namespace fsv
