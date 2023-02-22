#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using edit_imgui_signature = void(entt::handle);
	struct edit_imgui : base_function<edit_imgui_signature> {};
}

#define refltype kengine::meta::edit_imgui
kengine_function_reflection_info;
#undef refltype
