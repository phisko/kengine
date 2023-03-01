#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::scripting::lua {
	template<typename Func>
	void register_function(const entt::registry & r, const char * name, Func && func) noexcept;
}

#include "register_function.inl"