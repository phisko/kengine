#pragma once

#ifdef KENGINE_LUA

// putils
#include "lua_helper.hpp"

// kengine helpers
#include "helpers/scriptLanguageHelper.hpp"

// kengine data
#include "data/LuaStateComponent.hpp"

namespace kengine::luaHelper {
	template<typename ... Types>
	void registerTypes() noexcept;

	template<typename ... Comps>
	void registerComponents() noexcept;

	template<typename Func>
	void registerFunction(const char * name, Func && func) noexcept;
}

#include "luaHelper.inl"

#endif