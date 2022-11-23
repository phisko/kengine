#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ... Comps>
	void registerLoadFromJSON(entt::registry & r) noexcept;
}

#include "registerLoadFromJSON.inl"
