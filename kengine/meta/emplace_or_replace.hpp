#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using emplace_or_replace_signature = void(entt::handle, const void * comp);
	struct emplace_or_replace : functions::base_function<emplace_or_replace_signature> {};

	using emplace_or_replace_move_signature = void(entt::handle, void * comp);
	struct emplace_or_replace_move : functions::base_function<emplace_or_replace_move_signature> {};
}

#define refltype kengine::meta::emplace_or_replace
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype

#define refltype kengine::meta::emplace_or_replace_move
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype