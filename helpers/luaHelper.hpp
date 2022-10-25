#pragma once

#ifdef KENGINE_LUA

#include "scriptLanguageHelper.hpp"
#include "data/LuaStateComponent.hpp"
#include "lua_helper.hpp"

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