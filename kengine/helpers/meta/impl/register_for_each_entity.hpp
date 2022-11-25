#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ... Comps>
	void register_for_each_entity(entt::registry & r) noexcept;
}

#include "register_for_each_entity.inl"