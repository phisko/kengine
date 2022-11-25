#pragma once

// nlohmann
#include <nlohmann/json.hpp>

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine {
	namespace meta {
		struct save_to_json : functions::base_function<
			nlohmann::json(entt::const_handle)
		> {};
	}
}

#define refltype kengine::meta::save_to_json
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype
