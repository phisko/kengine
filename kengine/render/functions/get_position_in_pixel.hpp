#pragma once

// putils
#include "putils/point.hpp"

// kengine
#include "kengine/base_function.hpp"

namespace kengine::render {
	using get_position_in_pixel_signature = std::optional<putils::point3f>(entt::entity window, const putils::point2ui & pixel);
	//! putils reflect all
	//! parents: [refltype::base]
	struct get_position_in_pixel : base_function<get_position_in_pixel_signature> {};
}

#include "get_position_in_pixel.rpp"