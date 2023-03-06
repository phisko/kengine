#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using count_signature = size_t(entt::registry &);
	//! putils reflect all
	//! parents: [refltype::base]
	struct count : base_function<count_signature> {};
}

#include "count.rpp"