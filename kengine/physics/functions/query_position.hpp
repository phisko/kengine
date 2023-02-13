#pragma once

// putils
#include "putils/point.hpp"

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::functions {
	using query_position_signature = void(const putils::point3f & pos, float radius, const entity_iterator_func & func);
	struct query_position : base_function<query_position_signature> {};
}

#define refltype kengine::functions::query_position
kengine_function_reflection_info;
#undef refltype