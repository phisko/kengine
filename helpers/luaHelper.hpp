#pragma once

#ifdef KENGINE_LUA

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "lua_helper.hpp"

// kengine helpers
#include "helpers/scriptLanguageHelper.hpp"

// kengine data
#include "data/LuaStateComponent.hpp"

namespace kengine::luaHelper {
	template<typename ... Types>
	void registerTypes(const entt::registry & r) noexcept;

	template<typename ... Comps>
	void registerComponents(entt::registry & r) noexcept;

	template<typename Func>
	void registerFunction(const entt::registry & r, const char * name, Func && func) noexcept;
}

#include "luaHelper.inl"

#endif