#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta::imgui {
	using display_signature = void(entt::const_handle);
	//! putils reflect all
	//! parents: [refltype::base]
	struct display : base_function<display_signature> {};
}

#include "display.rpp"