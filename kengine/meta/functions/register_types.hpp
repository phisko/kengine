#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::functions {
	using register_types_signature = void(entt::registry & r);

	//! putils reflect all
	//! parents: [refltype::base]
	struct pre_register_types : base_function<register_types_signature> {};

	//! putils reflect all
	//! parents: [refltype::base]
	struct register_types : base_function<register_types_signature> {};
}

#include "register_types.rpp"