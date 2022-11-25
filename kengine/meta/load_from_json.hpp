#pragma once

// nlohmann
#include <nlohmann/json.hpp>

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	struct load_from_json : functions::base_function<
		void(const nlohmann::json &, entt::handle)
	> {};
}

#define refltype kengine::meta::load_from_json
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype