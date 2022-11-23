#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ... Comps>
	void registerCount(const entt::registry & r) noexcept;
}

#include "registerCount.inl"