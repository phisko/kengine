#pragma once

// entt
#include <entt/entity/fwd.hpp>

#ifndef KENGINE_SCRIPT_SYSTEM_MAX_FUNCTION_SIZE
#define KENGINE_SCRIPT_SYSTEM_MAX_FUNCTION_SIZE 64
#endif

namespace kengine::scripting {
	template<typename Func, typename Func2>
	void init_bindings(entt::registry & r, Func && register_function, Func2 && register_type) noexcept;
}

#include "init_bindings.inl"