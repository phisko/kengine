#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename... Comps>
	void register_match_string(entt::registry & r) noexcept;
}

#include "register_match_string.inl"
