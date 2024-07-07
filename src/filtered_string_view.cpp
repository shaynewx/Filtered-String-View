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

	// 2.4.5 带有谓词的以空字符结尾的字符串构造函数
	filtered_string_view::filtered_string_view(const char* str, filter predicate)
	: pointer_(nullptr)
	, length_(0)
	, predicate_(std::move(predicate)) {
		while (*str) { // 当未到达字符串末尾（null 字符）
			if (predicate_(*str)) { // 如果当前字符符合谓词条件
				if (!pointer_) {
					pointer_ = str; // 第一次找到符合条件的字符，设置 pointer_ 指向这个字符
				}
				length_++; // 递增符合条件的字符数量
			}
			str++; // 移动到下一个字符
		}
	}

	// 2.4.6 拷贝构造函数
	filtered_string_view::filtered_string_view(const filtered_string_view& other)
	: pointer_(other.pointer_)
	, length_(other.length_)
	, predicate_(other.predicate_) {
		// 由于 filtered_string_view 不拥有数据，直接复制指针和长度即可
	}

	// 2.4.6 移动构造函数
	filtered_string_view::filtered_string_view(filtered_string_view&& other) noexcept
	: pointer_(other.pointer_)
	, length_(other.length_)
	, predicate_(std::move(other.predicate_)) {
		// 将原对象的资源转移给新对象
		other.pointer_ = nullptr; // 确保原对象不再指向原始数据
		other.length_ = 0; // 清空原对象的长度
	}

	// 运算符重载的实现
	// 2.5.2 ==运算符的重载，用于比较两个视图是否指向相同的数据序列（通过比较指针）并且长度相同 （用于更新已存在的对象）
	auto filtered_string_view::operator==(const filtered_string_view& other) const -> bool {
		return (pointer_ == other.pointer_ && length_ == other.length_);
	}

	// 2.5.3 =运算符的重载，用于将一个即将被销毁的对象的资源移动到另一个对象 （在已存在的对象间转移）
	auto filtered_string_view::operator=(filtered_string_view&& other) noexcept -> filtered_string_view& {
		if (this != &other) { // 避免自赋值
			pointer_ = other.pointer_;
			length_ = other.length_;
			predicate_ = std::move(other.predicate_);

			// 将 other 重置为有效的默认状态
			other.pointer_ = nullptr;
			other.length_ = 0;
		}
		return *this; // 返回当前对象的引用
	}

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
