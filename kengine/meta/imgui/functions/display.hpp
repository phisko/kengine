#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta::imgui {
	using display_signature = void(entt::const_handle);
	struct display : base_function<display_signature> {};
}

#define refltype kengine::meta::imgui::display
kengine_function_reflection_info;
#undef refltype