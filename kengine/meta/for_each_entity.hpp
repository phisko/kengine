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
kengine_function_reflection_info;
#undef refltype

#define refltype kengine::meta::for_each_entity_without
kengine_function_reflection_info;
#undef refltype