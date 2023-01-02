#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename... Comps>
	void register_detach_from(entt::registry & r) noexcept;
}

#include "register_detach_from.inl"