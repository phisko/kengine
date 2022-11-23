#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ... Comps>
	void registerDetachFrom(entt::registry & r) noexcept;
}

#include "registerDetachFrom.inl"