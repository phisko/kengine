#pragma once

// putils
#include "putils/point.hpp"

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::physics {
	using query_position_signature = void(const putils::point3f & pos, float radius, const entity_iterator_func & func);
	//! putils reflect all
	//! parents: [refltype::base]
	struct query_position : base_function<query_position_signature> {};
}

#include "query_position.rpp"