#pragma once

#include "BaseFunction.hpp"

namespace kengine {
	class Entity;

	namespace meta {
		struct EditImGui : functions::BaseFunction<
			void(kengine::Entity &)
		> {};
	}
}

#define refltype kengine::meta::EditImGui
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype
