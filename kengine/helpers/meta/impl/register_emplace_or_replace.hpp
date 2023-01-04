#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename... Comps>
	void register_emplace_or_replace(entt::registry & r) noexcept;
}

#include "register_emplace_or_replace.inl"