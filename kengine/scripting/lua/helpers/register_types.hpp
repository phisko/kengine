#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::scripting::lua {
	template<bool IsComponent, typename... Types>
	void register_types(entt::registry & r) noexcept;
}

#include "register_types.inl"