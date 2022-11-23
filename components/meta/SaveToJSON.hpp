#pragma once

// nlohmann
#include <nlohmann/json.hpp>

// kengine functions
#include "BaseFunction.hpp"

namespace kengine {
	namespace meta {
		struct SaveToJSON : functions::BaseFunction<
			nlohmann::json(entt::const_handle)
		> {};
	}
}

#define refltype kengine::meta::SaveToJSON
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype
