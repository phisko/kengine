#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::functions {
	using on_mouse_captured_signature = void(entt::entity window, bool captured);
	struct on_mouse_captured : base_function<on_mouse_captured_signature> {};
}

#define refltype kengine::functions::on_mouse_captured
kengine_function_reflection_info;
#undef refltype