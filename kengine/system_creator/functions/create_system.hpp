#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::system_creator {
	using create_system_signature = entt::entity(entt::registry &);
	//! putils reflect all
	//! parents: [refltype::base]
	struct create_system : base_function<create_system_signature> {};
}

#include "create_system.rpp"