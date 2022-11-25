#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ... Comps>
	void register_move(entt::registry & r) noexcept;
}

#include "register_move.inl"
