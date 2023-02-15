#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::imgui_lua_helper {
	KENGINE_SCRIPTING_LUA_IMGUI_BINDINGS_EXPORT void init_bindings(const entt::registry & r) noexcept;
}