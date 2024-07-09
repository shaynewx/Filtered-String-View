#include "./filtered_string_view.h"
#include <algorithm>
#include <compare>
#include <cstring>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>

namespace fsv {

	// 默认的 静态成员 谓词函数，总是返回 true
	auto filtered_string_view::default_predicate(const char&) -> bool {
		return true;
	}

	// 构造函数的实现
	// 2.4.1 默认构造函数
	filtered_string_view::filtered_string_view()
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
	: pointer_(s.data())
	, length_(s.size()) // 表示开始时认为没有任何字符符合谓词条件
	, predicate_(std::move(predicate)) {}

	// 2.4.4 隐式以空字符结尾的字符串构造函数
	filtered_string_view::filtered_string_view(const char* str)
	: pointer_(str) // 直接指向传入的 C 风格字符串
	, length_(std::strlen(str)) // 使用 strlen 计算字符串长度，（字符串以 null 结尾）
	, predicate_(default_predicate) {} // 使用默认的总是返回 true 的谓词

	// 2.4.5 带有谓词的以空字符结尾的字符串构造函数
	filtered_string_view::filtered_string_view(const char* str, filter predicate)
	: pointer_(str) // 直接指向原始 C 字符串
	, length_(std::strlen(str)) // 开始时认为没有任何字符符合谓词条件
	, predicate_(std::move(predicate)) {}

	// 2.4.6 拷贝构造函数
	filtered_string_view::filtered_string_view(const filtered_string_view& other)
	: pointer_(other.pointer_)
	, length_(other.length_)
	, predicate_(other.predicate_) {} // 由于 filtered_string_view 不拥有数据，直接复制other的数据即可

	// 2.4.6 移动构造函数
	filtered_string_view::filtered_string_view(filtered_string_view&& other) noexcept
	: pointer_(other.pointer_)
	, length_(other.length_)
	, predicate_(std::move(other.predicate_)) { // 将原对象other的资源转移给新对象
		other.pointer_ = nullptr; // 确保原对象other的指针不再指向原始数据
		other.length_ = 0; // 清空原对象other的长度
	}

	// 2.5 类内部运算符重载的实现
	// 2.5.3 =运算符的重载，用于将一个即将被销毁的对象的资源移动到另一个对象 （在已存在的对象间转移）
	auto filtered_string_view::operator=(filtered_string_view&& other) noexcept -> filtered_string_view& {
		if (this != &other) { // 于确保对象不会将自己赋值给自己（this 用于获取当前对象的指针）
			pointer_ = other.pointer_;
			length_ = other.length_;
			predicate_ = std::move(other.predicate_);

			// 将 other 重置为有效的默认状态
			other.pointer_ = nullptr;
			other.length_ = 0;
		}
		return *this; // 返回 接受资源移动的当前对象 的引用
	}

	// 定义静态成员，用于无效索引情况的默认字符
	const char filtered_string_view::default_char = '\0';

	// 2.5.4 []运算符的重载，用于返回类实例中特定索引位置的字符
	auto filtered_string_view::operator[](int n) const -> const char& {
		const char* temp_ptr = pointer_; // 从原始数据开始
		int count = 0; // 跟踪已经遇到的符合谓词条件的字符数

		while (*temp_ptr != '\0') { // 循环遍历字符串
			if (predicate_(*temp_ptr)) { // 检查当前字符是否满足过滤条件
				if (count == n) { // 当找到第n个符合条件的字符时返回
					return *temp_ptr;
				}
				count++;
			}
			temp_ptr++; // 移动到下一个字符
		}

		// 如果索引n超出了符合条件的字符数，抛出异常
		//		throw std::out_of_range("filtered_string_view::operator[]: Index out of range");
		return default_char; // 如果索引n超出了符合条件的字符数，返回默认字符引用
	}

	// 2.5.5 类型转换运算符，允许 filtered_string_view 对象显式转换为 std::string，返回的是过滤后字符串的拷贝
	filtered_string_view::operator std::string() const {
		std::string result; // 初始化输出结果
		result.reserve(length_); // 结果的最大可能长度为原字符串的长度

		// 使用 copy_if 和 back_inserter 来将所有符合谓词条件的字符添加到 result 字符串中
		// 从 pointer_ 指向的起始位置开始，到 pointer_ 加上 length_ 的位置结束
		// 在目标容器 result（一个 std::string 对象）的末尾添加符合过滤条件的元素
		std::copy_if(pointer_, pointer_ + length_, std::back_inserter(result), predicate_);

		return result;
	}

	// 成员函数的实现
	// 2.6.1 at：允许根据索引从过滤后的字符串中读取一个字符
	auto filtered_string_view::at(int index) -> const char& {
		if (index < 0 || index >= static_cast<int>(size())) {
			throw std::domain_error("filtered_string_view::at: Invalid index");
		}

		const char* temp_ptr = pointer_; // 初始化指针，指向底层字符串数据的起始位置
		int count = 0; // 记录遇到符合谓词条件的字符的数量

		while (*temp_ptr != '\0') { // 循环遍历底层字符串
			if (predicate_(*temp_ptr)) { // 检查当前字符是否满足过滤条件
				if (count == index) { // 找到索引 index 所指的符合条件的字符时返回
					return *temp_ptr;
				}
				count++;
			}
			temp_ptr++;
		}

		// 理论上，这个异常不会被抛出，因为此前已经检查过
		//		throw std::out_of_range("filtered_string_view::at: Index out of range");
		return default_char; // 如果索引n超出了符合条件的字符数，返回默认字符引用
	}

	// 2.6.2 返回过滤后的字符串视图的长度，即满足谓词条件的字符总数
	//	auto filtered_string_view::size() const -> std::size_t {
	//		return length_;
	//	}
	auto filtered_string_view::size() const -> std::size_t {
		std::size_t count = 0;
		for (std::size_t i = 0; i < length_; ++i) {
			if (predicate_(pointer_[i])) {
				count++;
			}
		}
		return count;
	}

	// 2.6.3 返回过滤后的字符串是否为空
	auto filtered_string_view::empty() const -> bool {
		return size() == 0;
	}

	// 2.6.4 返回指向底层数据的指针
	auto filtered_string_view::data() const -> const char* {
		return pointer_;
	}

	// 2.6.5 访问用于进行过滤的谓词
	auto filtered_string_view::predicate() const -> const filter& {
		return predicate_;
	}

	// 2.7.1. ==运算符的重载，按字典顺序比较两个filtered_string_view字符串是否相等
	auto operator==(const filtered_string_view& lhs, const filtered_string_view& rhs) -> bool {
		return static_cast<std::string>(lhs) == static_cast<std::string>(rhs);
	}

	// 2.7.2 <=>运算符的重载
	auto operator<=>(const filtered_string_view& lhs, const filtered_string_view& rhs) -> std::strong_ordering {
		auto lhs_str = static_cast<std::string>(lhs); // 将 lhs 转换为 string
		auto rhs_str = static_cast<std::string>(rhs); // 将 rhs 转换为 string
		return lhs_str.compare(rhs_str) <=> 0; // 使用标准库的比较功能和C++20的太空船运算符
	}

	// 2.7.3 <<运算符的重载
	std::ostream& operator<<(std::ostream& os, const filtered_string_view& fsv) {
		for (std::size_t i = 0; i < fsv.size(); ++i) {
			os << fsv[static_cast<int>(i)]; // 输出每个过滤后的字符
		}
		return os;
	}

	// 2.8 类外部的非成员函数
	// 2.8.1 Compose
	auto compose(const filtered_string_view& fsv, const std::vector<filter>& filts) -> filtered_string_view {
		/*
		 * 接收一个fsv对象和一个过滤谓词的vector，返回一个新的fsv对象，
		 * 将所有过滤谓词用and组合，只有当所有谓词返回true时才返回true，也即所有过滤器会过滤相同字符
		 * 如果有一个过滤器是false，则符合谓词就会立即返回false并停止进一步检查
		 * */
		auto composite_filter = [filts](const char& c) -> bool {
			for (auto& filt : filts) {
				if (!filt(c)) {
					return false; // 短路，如果有谓词是false则返回
				}
			}
			return true; // 所有谓词都返回 true
		};

		return filtered_string_view(fsv.data(), composite_filter);
	}

	// 2.8.2 Split

} // namespace fsv
