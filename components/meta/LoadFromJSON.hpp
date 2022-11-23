#pragma once

// nlohmann
#include <nlohmann/json.hpp>

// kengine functions
#include "BaseFunction.hpp"

namespace kengine::meta {
	struct LoadFromJSON : functions::BaseFunction<
		void(const nlohmann::json &, entt::handle)
	> {};
}

#define refltype kengine::meta::LoadFromJSON
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype