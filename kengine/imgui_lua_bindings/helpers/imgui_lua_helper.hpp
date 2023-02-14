#pragma once

#ifdef KENGINE_LUA

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::imgui_lua_helper {
	KENGINE_CORE_EXPORT void init_bindings(const entt::registry & r) noexcept;
}

#endif