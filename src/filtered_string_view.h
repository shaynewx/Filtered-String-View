#ifndef COMP6771_ASS2_FSV_H
#define COMP6771_ASS2_FSV_H

#include <compare>
#include <functional>
#include <iterator>
#include <optional>
#include <string>

namespace fsv {
	class filtered_string_view {
		using filter = std::function<bool(const char&)>; // 别名定义，方便引用谓词类型

	 public:
		static bool default_predicate(const char&) {
			return true; // 默认谓词函数，总是返回 true
		}

		class iter { // class iter 是一个嵌套在 filtered_string_view 内部的迭代器类，用于遍历过滤后的字符串视图
		 public:
			using MEMBER_TYPEDEFS_GO_HERE = void; // 占位符，此处需要添加必要的类型定义
			using difference_type = std::ptrdiff_t;
			using value_type = const char;
			using pointer = const char*;
			using reference = const char&;
			using iterator_category = std::bidirectional_iterator_tag;

			iter(); // 迭代器的默认构造函数，具体实现将在类外定义

			const char& operator*() const; // 重载解引用运算符 （需要修改返回类型）
			const char* operator->() const; // 重载解箭头运算符 （需要修改返回类型）

			iter& operator++(); // 前置自增运算符，用于迭代器前进
			iter operator++(int); // 后置自增运算符，用于迭代器前进
			iter& operator--(); // 前置自减运算符，用于迭代器后退
			iter operator--(int); // 后置自减运算符，用于迭代器后退

			friend bool operator==(const iter&, const iter&); // 迭代器友元函数，重载等于比较运算符
			friend bool operator!=(const iter&, const iter&); // 迭代器友元函数，重载不等于比较运算符

		 private:
			/* Implementation-specific private members */
			// 此处添加迭代器的私有成员
		};

	 private:
	};
} // namespace fsv

#endif // COMP6771_ASS2_FSV_H
