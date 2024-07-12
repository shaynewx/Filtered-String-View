#include "./filtered_string_view.h"
#include <sstream>

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
	, length_(std::strlen(str)) // 使用 strlen 计算字符串长度
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
	// 2.5.2 =运算符的重载，用于复制任务
	auto filtered_string_view::operator=(const filtered_string_view& other) -> filtered_string_view& {
		if (this != &other) { // 检查自赋值
			pointer_ = other.pointer_;
			length_ = other.length_;
			predicate_ = other.predicate_; // 谓词的赋值通常是安全的
		}
		return *this;
	}
	// 2.5.3 =运算符的重载，用于移动任务
	auto filtered_string_view::operator=(filtered_string_view&& other) noexcept -> filtered_string_view& {
		if (this != &other) {
			pointer_ = other.pointer_;
			length_ = other.length_;
			predicate_ = std::move(other.predicate_); // 移动谓词

			// 将移动来源对象置于安全状态
			other.pointer_ = nullptr;
			other.length_ = 0;
		}
		return *this;
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
			std::ostringstream oss;
			oss << "filtered_string_view::at(" << index << "): invalid index";
			throw std::domain_error(oss.str());
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

		return default_char; // 如果索引n超出了符合条件的字符数，返回默认字符引用
	}

	// 2.6.2 返回过滤后的字符串视图的长度，即满足谓词条件的字符总数
	auto filtered_string_view::size() const -> std::size_t {
		return static_cast<std::size_t>(std::count_if(pointer_, pointer_ + length_, predicate_));
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

	// 返回初始字符串的长度
	auto filtered_string_view::original_size() const -> std::size_t {
		return length_;
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
	// 接收一个fsv对象和一个过滤谓词的vector，返回一个新的fsv对象，
	// 将所有过滤谓词用and组合，只有当所有谓词返回true时才返回true，也即所有过滤器会过滤相同字符
	// 但是只要有一个谓词函数是false，则这个新的谓词函数就会短路，且立即返回false并停止进一步检查（类似 logical AND）
	auto compose(const filtered_string_view& fsv, const std::vector<filter>& filts) -> filtered_string_view {
		auto composite_filter = [filts](const char& c) -> bool {
			return std::ranges::all_of(filts, [&](const auto& filt) { return filt(c); });
		};

		return filtered_string_view(fsv.data(), composite_filter);
	}

	// 2.8.2 Split
	// 接收两个filtered_string_view作为参数，一个是fsv，一个是tok，返回一个vector
	// 使用tok作为分隔符，将fsv切割成一系列子串并返回
	// 通常来说，分隔符出现在两个strings的中间，也有可能会出现在字符串的开头或结尾，这种情况意味着分割后的结果可能包含空的filtered_string_view
	// 如果fsv中不包含tok，或fsv为空，则返回的vector中只包含一个元素，即原始fsv的副本
	// split类似python中的spilt()函数，但与python不同的是，但是fsv::split()可以接受空的分隔符
	auto split(const filtered_string_view& fsv, const filtered_string_view& tok) -> std::vector<filtered_string_view> {
		std::vector<filtered_string_view> result;

		// 如果fsv为空或者tok为空，返回fsv副本
		if (fsv.empty() or tok.empty()) {
			result.push_back(fsv);
			return result;
		}

		const char* start = fsv.data(); // 指向 fsv 的起始字符
		const char* end = start + fsv.original_size(); // 指向 fsv 的末尾字符串的下一位
		const char* current = start; // 用于遍历 fsv 的指针
		const char* tok_start = tok.data(); // 指向分隔符 tok 的起始字符
		const std::size_t tok_len = tok.size(); // 获取分隔符的长度

		while (current < end) { // 遍历 fsv
			const char* next = std::search(current, end, tok_start, tok_start + tok_len); // 查找分隔符的位置

			if (next == end) { // 如果没有找到分隔符（fsv不包含tok），说明当前段落是最后一部分
				if (current != end) {
					// 创建一个新的以空字符结尾的字符串
					std::size_t len = static_cast<size_t>(end - current);
					char* temp = new char[len + 1];
					std::strncpy(temp, current, len);
					temp[len] = '\0';
					result.emplace_back(temp, fsv.predicate());
				}
				break;
			}
			else {
				// 创建一个新的以空字符结尾的字符串
				std::size_t len = static_cast<size_t>(next - current);
				char* temp = new char[len + 1];
				std::strncpy(temp, current, len);
				temp[len] = '\0';
				result.emplace_back(temp, fsv.predicate());
				current = next + tok_len; // 更新 current，跳过当前找到的分隔符
			}
		}

		// 特殊情况，如 fsv 以分隔符结尾
		if (current == end && end != start && *(end - tok_len) == *tok_start) {
			result.emplace_back("", fsv.predicate());
		}
		return result;
	}

	// 2.8.3 substr
	// 接收三个传入参数，fsv，pos和count
	// 返回一个新的filtered_string_view，与fsv有同样的底层函数，但是呈现原字符串的“子字符串”视图
	// 这个子字符串从pos开始，并且长度为rcount，rcount = count <= 0 ? size() - pos() : count
	// 如果传入的 count 参数小于或等于 0，或者 count 大于从 pos 开始到字符串末尾的字符数，
	// 则 rcount 会被计算为从 pos到原字符串末尾的长度（即 size() - pos()。
	// 如果 count 大于 0 且小于或等于从 pos 开始到字符串末尾的字符数，
	// rcount将直接等于 count 也即这个子字符串提供了 fsv的 [pos, pos + rcount] 的视图
	// 这意味着视图将包括原字符串中从位置 pos开始的、连续 rcount 个满足谓词条件的字符
	// 子字符串是有可能长度为0的，在这种情况下，返回的filtered_string_view是一个""
	auto substr(const filtered_string_view& fsv, int pos, int count) -> filtered_string_view {
		const char* start = fsv.data();
		const char* end = start + fsv.original_size(); // 使用底层字符串的长度
		const char* current = start;

		// 确保 pos 不超过过滤后的字符串长度
		if (pos >= static_cast<int>(fsv.size())) {
			return filtered_string_view("", fsv.predicate());
		}

		// 查找子字符串的起始位置
		int filtered_pos = 0;
		const char* substr_start = nullptr;

		while (current != end and filtered_pos < pos) {
			if (fsv.predicate()(*current)) {
				++filtered_pos;
			}
			++current;
		}

		// 确保找到起始位置
		if (current == end) {
			return filtered_string_view("", fsv.predicate());
		}

		substr_start = current;

		// 计算 rcount，并查找子字符串的结束位置
		int filtered_count = 0;
		const char* substr_end = nullptr;

		while (current != end && (count <= 0 || filtered_count < count)) {
			if (fsv.predicate()(*current)) {
				++filtered_count;
			}
			++current;
		}

		substr_end = current;

		// 创建一个新的以空字符结尾的字符串
		std::size_t len = static_cast<size_t>(substr_end - substr_start);
		char* temp = new char[len + 1];
		std::strncpy(temp, substr_start, len);
		temp[len] = '\0';

		return filtered_string_view(temp, fsv.predicate());
	}

	// 2.9 迭代器 Iterator
	// 允许调用者按照字符逐个迭代过滤出的单词，仅仅需要实现一个双向迭代器，且它应该支持双向迭代器支持的所有操作符
	// 这个迭代器也是个常量迭代器，这意味这即使使用非常量的filtered_string_view，也不会改变underlying filtered_string
	// 迭代器的 value_type 应为 char
	// 迭代器的 reference 类型应该是 const char&，表示迭代器返回的是字符的常量引用。
	// 迭代器的 pointer 类型应为 void。这通常表示迭代器不提供直接的指针访问功能。
	// 迭代器应该有一个 public default constructor
	// 迭代器不应允许对底层的字符串数据进行修改
	// 您还可以定义私有的、特定于实现的构造函数
	// 您可以假设任何改变 filtered_string_view 的方法都会使任何迭代器无效
	// filtered_string_view 应该有一个 member type 为 iterator = const_iterator

	// 迭代器的初始实现
	filtered_string_view::const_iterator::const_iterator()
	: ptr_(nullptr)
	, predicate_(nullptr) {}

	filtered_string_view::const_iterator::const_iterator(const char* ptr, const filter& predicate)
	: ptr_(ptr)
	, predicate_(&predicate) {
		while (ptr_ && *ptr_ && !(*predicate_)(*ptr_)) {
			++ptr_;
		}
	}

	// 运算符重载
	auto filtered_string_view::const_iterator::operator*() const -> reference {
		return *ptr_;
	}

	auto filtered_string_view::const_iterator::operator++() -> const_iterator& {
		do {
			++ptr_;
		} while (ptr_ && *ptr_ && !(*predicate_)(*ptr_)); // 跳过所有不符合谓词条件的字符
		return *this;
	}

	auto filtered_string_view::const_iterator::operator++(int) -> const_iterator {
		const_iterator tmp = *this;
		++(*this);
		return tmp;
	}

	auto filtered_string_view::const_iterator::operator--() -> const_iterator& {
		do {
			--ptr_;
		} while (ptr_ && *ptr_ && !(*predicate_)(*ptr_)); // 跳过所有不符合谓词条件的字符
		return *this;
	}

	auto filtered_string_view::const_iterator::operator--(int) -> const_iterator {
		const_iterator tmp = *this;
		--(*this);
		return tmp;
	}

	auto filtered_string_view::const_iterator::operator==(const const_iterator& other) const -> bool {
		return ptr_ == other.ptr_;
	}

	auto filtered_string_view::const_iterator::operator!=(const const_iterator& other) const -> bool {
		return ptr_ != other.ptr_;
	}

	// 2.10 begin()、end()、cbegin()、cend()、rbegin()、rend()、crbegin()、crend()
	auto filtered_string_view::begin() const -> const_iterator {
		const char* ptr = pointer_;
		while (ptr != pointer_ + length_ && !predicate_(*ptr)) {
			++ptr;
		}
		return const_iterator(ptr, predicate_);
	}

	auto filtered_string_view::cbegin() const -> const_iterator {
		return begin();
	}

	auto filtered_string_view::end() const -> const_iterator {
		return const_iterator(pointer_ + length_, predicate_);
	}

	auto filtered_string_view::cend() const -> const_iterator {
		return end();
	}

	auto filtered_string_view::rbegin() const -> const_reverse_iterator {
		return const_reverse_iterator(end());
	}

	auto filtered_string_view::crbegin() const -> const_reverse_iterator {
		return rbegin();
	}

	auto filtered_string_view::rend() const -> const_reverse_iterator {
		return const_reverse_iterator(begin());
	}

	auto filtered_string_view::crend() const -> const_reverse_iterator {
		return rend();
	}
} // namespace fsv
