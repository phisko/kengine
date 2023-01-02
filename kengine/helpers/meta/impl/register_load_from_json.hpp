#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename... Comps>
	void register_load_from_json(entt::registry & r) noexcept;
}

#include "register_load_from_json.inl"
