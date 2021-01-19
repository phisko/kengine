#pragma once

#include "BaseFunction.hpp"
#include "json.hpp"

namespace kengine {
	class Entity;

	namespace meta {
		struct SaveToJSON : functions::BaseFunction<
			putils::json(const Entity &)
		> {};
	}
}

#define refltype kengine::meta::SaveToJSON
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype
