#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename... Comps>
	void register_count(const entt::registry & r) noexcept;
}

#include "register_count.inl"