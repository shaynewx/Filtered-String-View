#include "./filtered_string_view.h"
#include <iostream>
// Implement here

namespace fsv {
	// 默认的 静态成员 谓词函数，总是返回 true
	auto filtered_string_view::default_predicate(const char&) -> bool {
		return true;
	}

	// 构造函数的实现
	filtered_string_view::filtered_string_view() // 2.4.1
	: pointer_(nullptr)
	, length_(0)
	, predicate_(default_predicate) {}

	// 成员函数的实现
	auto filtered_string_view::size() const -> size_t {
		return length_;
	}
} // namespace fsv
