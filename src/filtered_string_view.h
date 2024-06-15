#ifndef COMP6771_ASS2_FSV_H
#define COMP6771_ASS2_FSV_H

#include <compare>
#include <functional>
#include <iterator>
#include <optional>
#include <string>

namespace fsv {
	class filtered_string_view {
		class iter {
		 public:
			using MEMBER_TYPEDEFS_GO_HERE = void;

			iter();

			auto operator*() const -> void; // change this
			auto operator->() const -> void; // change this

			auto operator++() -> iter&;
			auto operator++(int) -> iter;
			auto operator--() -> iter&;
			auto operator--(int) -> iter;

			friend auto operator==(const iter&, const iter&) -> bool;
			friend auto operator!=(const iter&, const iter&) -> bool;

		 private:
			/* Implementation-specific private members */
		};

	 public:
	 private:
	};
} // namespace fsv

#endif // COMP6771_ASS2_FSV_H
