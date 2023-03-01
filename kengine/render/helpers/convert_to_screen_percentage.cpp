#include "convert_to_screen_percentage.hpp"

// magic_enum
#include <magic_enum.hpp>

// kengine
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::render {
	template<size_t N>
	static putils::rect<float, N> convert_to_screen_percentage_impl(const putils::rect<float, N> & rect, const putils::point2f & screen_size, const on_screen & comp) noexcept {
		KENGINE_PROFILING_SCOPE;

		switch (comp.coordinates) {
			case on_screen::coordinate_type::pixels: {
				putils::rect<float, N> ret = rect;
				ret.position.x /= screen_size.x;
				ret.position.y /= screen_size.y;
				ret.size.x /= screen_size.x;
				ret.size.y /= screen_size.y;
				return ret;
			}
			case on_screen::coordinate_type::screen_percentage:
				return rect;
			default:
				static_assert(magic_enum::enum_count<on_screen::coordinate_type>() == 2);
				return rect;
		}
	}

	putils::rect3f convert_to_screen_percentage(const putils::rect3f & rect, const putils::point2f & screen_size, const on_screen & comp) noexcept {
		return convert_to_screen_percentage_impl(rect, screen_size, comp);
	}

	putils::rect2f convert_to_screen_percentage(const putils::rect2f & rect, const putils::point2f & screen_size, const on_screen & comp) noexcept {
		return convert_to_screen_percentage_impl(rect, screen_size, comp);
	}
}