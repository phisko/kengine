#pragma once

#ifdef KENGINE_LUA

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/script_language_helper.hpp"

// kengine data
#include "kengine/data/lua_state.hpp"

namespace kengine::lua_helper {
	template<typename ... Types>
	void register_types(const entt::registry & r) noexcept;

	template<typename ... Comps>
	void register_components(entt::registry & r) noexcept;

	template<typename Func>
	void register_function(const entt::registry & r, const char * name, Func && func) noexcept;
}

#include "lua_helper.inl"

#endif