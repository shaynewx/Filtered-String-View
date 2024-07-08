#include "./filtered_string_view.h"
#include <algorithm>
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
	: pointer_(s.data())
	, length_(0) // 表示开始时认为没有任何字符符合谓词条件
	, predicate_(std::move(predicate)) {
		for (char c : s) {
			if (predicate_(c)) { // 如果当前字符满足为此条件，指针没有指向匹配字符
				length_++; // 只计算符合谓词的字符
			}
		}
	}

	// 2.4.4 隐式以空字符结尾的字符串构造函数
	filtered_string_view::filtered_string_view(const char* str)
	: pointer_(str) // 直接指向传入的 C 风格字符串
	, length_(std::strlen(str)) // 使用 strlen 计算字符串长度，（字符串以 null 结尾）
	, predicate_(default_predicate) {} // 使用默认的总是返回 true 的谓词

	// 2.4.5 带有谓词的以空字符结尾的字符串构造函数
	filtered_string_view::filtered_string_view(const char* str, filter predicate)
	: pointer_(str) // 直接指向原始 C 字符串
	, length_(0) // 开始时认为没有任何字符符合谓词条件
	, predicate_(std::move(predicate)) {
		const char* start = str; // 用一个额外的指针来迭代字符串，以保留原始指针
		while (*start) { // *str 返回str当前指向的字符
			if (predicate_(*start)) { // 检查当前字符是否满足谓词条件
				length_++; // 增加符合条件的字符计数
			}
			start++; // 移动到下一个字符
		}
	}

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

	// 运算符重载的实现
	// 2.5.2 ==运算符的重载，用于比较两个视图是否指向相同的数据序列（通过比较指针）并且长度相同 （用于更新已存在的对象）
	auto filtered_string_view::operator==(const filtered_string_view& other) const -> bool {
		return (pointer_ == other.pointer_ and length_ == other.length_);
	}

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
	//
	auto filtered_string_view::data() const -> const char* {
		return pointer_;
	}

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
	auto filtered_string_view::size() const -> std::size_t {
		return length_;
	}

	// 2.6.3 返回过滤后的字符串是否为空
	auto filtered_string_view::empty() const -> bool {
		return size() == 0;
	}

} // namespace fsv
