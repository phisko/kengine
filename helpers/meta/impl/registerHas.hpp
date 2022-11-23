#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ... Comps>
	void registerHas(entt::registry & r) noexcept;
}

#include "registerHas.inl"
