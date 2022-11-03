#pragma once

// kengine functions
#include "BaseFunction.hpp"

namespace kengine {
	class Entity;

	namespace meta {
		struct MatchString : functions::BaseFunction<
			bool(const Entity &, const char *)
		> {};
	}
}

#define refltype kengine::meta::MatchString
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype