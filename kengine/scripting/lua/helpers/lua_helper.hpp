#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine scripting
#include "kengine/scripting/helpers/script_language_helper.hpp"

// kengine scripting/lua
#include "kengine/scripting/lua/data/lua_state.hpp"

namespace kengine::lua_helper {
	template<bool IsComponent, typename... Types>
	void register_types(entt::registry & r) noexcept;

	template<typename Func>
	void register_function(const entt::registry & r, const char * name, Func && func) noexcept;
}

#include "lua_helper.inl"