#pragma once

#include <nlohmann/json.hpp>
#include "BaseFunction.hpp"

namespace kengine {
	class Entity;

	namespace meta {
		struct SaveToJSON : functions::BaseFunction<
			nlohmann::json(const Entity &)
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
