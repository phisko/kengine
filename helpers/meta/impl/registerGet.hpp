#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ... Comps>
	void registerGet(entt::registry & r) noexcept;
}

#include "registerGet.inl"
