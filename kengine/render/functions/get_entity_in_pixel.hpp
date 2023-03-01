#pragma once

// putils
#include "putils/point.hpp"

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::render {
	using get_entity_in_pixel_signature = entt::entity(entt::entity window, const putils::point2ui & pixel);
	//! putils reflect all
	//! parents: [refltype::base]
	struct get_entity_in_pixel : base_function<get_entity_in_pixel_signature> {};
}

#include "get_entity_in_pixel.rpp"