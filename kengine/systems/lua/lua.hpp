#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_LUA_EXPORT void add_lua(entt::registry & r) noexcept;
}