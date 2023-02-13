#pragma once

// putils
#include "putils/point.hpp"

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::functions {
	using get_position_in_pixel_signature = std::optional<putils::point3f>(entt::entity window, const putils::point2ui & pixel);
	struct get_position_in_pixel : base_function<get_position_in_pixel_signature> {};
}

#define refltype kengine::functions::get_position_in_pixel
kengine_function_reflection_info;
#undef refltype
