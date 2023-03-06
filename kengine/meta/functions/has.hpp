#pragma once

// kengine
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using has_signature = bool(entt::const_handle);
	//! putils reflect all
	//! parents: [refltype::base]
	struct has : base_function<has_signature> {};
}

#include "has.rpp"