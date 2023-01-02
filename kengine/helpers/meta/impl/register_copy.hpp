#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename... Comps>
	void register_copy(entt::registry & r) noexcept;
}

#include "register_copy.inl"
