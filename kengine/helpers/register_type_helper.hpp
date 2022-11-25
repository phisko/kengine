#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ... Comps>
	void register_components(entt::registry & r) noexcept;

	template<typename ...Types>
	void register_types(entt::registry & r) noexcept;

	template<typename F>
	void register_function(const entt::registry & r, const char * name, F && func) noexcept;
}

#include "register_type_helper.inl"
