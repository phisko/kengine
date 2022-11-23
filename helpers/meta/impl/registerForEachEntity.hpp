#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ... Comps>
	void registerForEachEntity(entt::registry & r) noexcept;
}

#include "registerForEachEntity.inl"