#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ... Comps>
	void registerCopy(entt::registry & r) noexcept;
}

#include "registerCopy.inl"
