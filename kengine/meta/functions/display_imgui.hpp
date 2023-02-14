#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using display_imgui_signature = void(entt::const_handle);
	struct display_imgui : functions::base_function<display_imgui_signature> {};
}

#define refltype kengine::meta::display_imgui
kengine_function_reflection_info;
#undef refltype