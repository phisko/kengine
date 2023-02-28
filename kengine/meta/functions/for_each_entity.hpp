#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine {
	namespace meta {
		using for_each_entity_signature = void(entt::registry & r, const entity_iterator_func & func);

		//! putils reflect all
		//! parents: [refltype::base]
		struct for_each_entity : base_function<for_each_entity_signature> {};

		//! putils reflect all
		//! parents: [refltype::base]
		struct for_each_entity_without : base_function<for_each_entity_signature> {};
	}
}

#include "for_each_entity.rpp"