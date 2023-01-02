#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::functions {
	using on_mouse_captured_signature = void(entt::entity window, bool captured);
	struct on_mouse_captured : base_function<on_mouse_captured_signature> {};
}

#define refltype kengine::functions::on_mouse_captured
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype