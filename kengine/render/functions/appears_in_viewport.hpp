#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::functions {
	using appears_in_viewport_signature = bool(entt::entity entity);
	//! putils reflect all
	//! parents: [refltype::base]
	struct appears_in_viewport : base_function<appears_in_viewport_signature> {};
}

#include "appears_in_viewport.rpp"