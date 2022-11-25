#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	struct edit_imgui : functions::base_function<
		void(entt::handle)
	> {};
}

#define refltype kengine::meta::edit_imgui
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype
