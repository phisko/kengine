#pragma once

#include <nlohmann/json.hpp>
#include "BaseFunction.hpp"

namespace kengine {
	class Entity;

	namespace meta {
		struct LoadFromJSON : functions::BaseFunction<
			void(const nlohmann::json &, Entity &)
		> {};
	}
}

#define refltype kengine::meta::LoadFromJSON
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype