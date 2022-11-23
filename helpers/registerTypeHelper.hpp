#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ... Comps>
	void registerComponents(entt::registry & r) noexcept;

	template<typename ...Types>
	void registerTypes(entt::registry & r) noexcept;

	template<typename F>
	void registerFunction(const entt::registry & r, const char * name, F && func) noexcept;
}

#include "registerTypeHelper.inl"
