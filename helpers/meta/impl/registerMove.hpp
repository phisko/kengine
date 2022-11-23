#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ... Comps>
	void registerMove(entt::registry & r) noexcept;
}

#include "registerMove.inl"
