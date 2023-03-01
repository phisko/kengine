#pragma once

// entt
#include <entt/entity/fwd.hpp>

#ifndef KENGINE_SCRIPT_SYSTEM_MAX_FUNCTION_SIZE
#define KENGINE_SCRIPT_SYSTEM_MAX_FUNCTION_SIZE 64
#endif

namespace kengine::script_language_helper {
	template<typename Func, typename Func2>
	void init(entt::registry & r, Func && register_function, Func2 && register_type) noexcept;

	template<typename T, typename Func, typename Func2>
	void register_component(entt::registry & r, Func && register_entity_member, Func2 && register_function) noexcept;
}

#include "script_language_helper.inl"