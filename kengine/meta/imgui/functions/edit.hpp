#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta::imgui {
	using edit_signature = void(entt::handle);
	struct edit : base_function<edit_signature> {};
}

#define refltype kengine::meta::imgui::edit
kengine_function_reflection_info;
#undef refltype
