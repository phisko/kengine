#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename... Comps>
	void register_meta_components(entt::registry & r) noexcept;
}

#include "register_meta_components.inl"