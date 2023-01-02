#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using display_imgui_signature = void(entt::const_handle);
	struct display_imgui : functions::base_function<display_imgui_signature> {};
}

#define refltype kengine::meta::display_imgui
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype