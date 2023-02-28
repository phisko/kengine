#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::physics {
	using on_collision_signature = void(entt::entity first, entt::entity second);
	//! putils reflect all
	//! parents: [refltype::base]
	struct on_collision : base_function<on_collision_signature> {};
}

#include "on_collision.rpp"