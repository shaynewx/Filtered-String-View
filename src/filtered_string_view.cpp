#include "./filtered_string_view.h"
#include <cstring>
#include <iostream>
#include <utility>

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

	// 2.4.2隐式字符串构造函数
	filtered_string_view::filtered_string_view(const std::string& s)
	: pointer_(s.data())
	, length_(s.size())
	, predicate_(default_predicate) {}

	// 2.4.3带Predicate的字符串构造函数
	filtered_string_view::filtered_string_view(const std::string& s, filter predicate)
	: pointer_(nullptr)
	, length_(0)
	, predicate_(std::move(predicate)) { // 将 predicate 参数转换为右值引用，然后其资源转移给成员变量 predicate_
		for (size_t i = 0; i < s.size(); ++i) {
			if (predicate_(s[i])) {
				if (!pointer_) {
					pointer_ = s.data() + i; // 让pointer指向第一次找到符合条件的字符
				}
				length_++; // 记录长度
			}
		}
	}

	// 2.4.4 隐式以空字符结尾的字符串构造函数
	filtered_string_view::filtered_string_view(const char* str)
	: pointer_(str) // 直接指向传入的 C 风格字符串
	, length_(std::strlen(str)) // 使用 strlen 计算字符串长度，（字符串以 null 结尾）
	, predicate_(default_predicate) // 使用默认的总是返回 true 的谓词
	{}

	// 成员函数的实现
	// 返回一个指向常量字符的指针，指向原始字符串中第一个满足谓词条件的字符
	auto filtered_string_view::data() const -> const char* {
		return pointer_;
	}

	// 返回过滤后的字符串视图的长度，即满足谓词条件的字符总数
	auto filtered_string_view::size() const -> size_t {
		return length_;
	}
} // namespace fsv
