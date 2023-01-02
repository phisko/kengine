#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine {
	namespace meta {
		using for_each_entity_signature = void(const entity_iterator_func & func);
		struct for_each_entity : functions::base_function<for_each_entity_signature> {};

		using for_each_entity_without_signature = void(const entity_iterator_func & func);
		struct for_each_entity_without : functions::base_function<for_each_entity_without_signature> {};
	}
}

#define refltype kengine::meta::for_each_entity
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype

#define refltype kengine::meta::for_each_entity_without
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype