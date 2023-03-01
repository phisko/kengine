#pragma once

// stl
#include <functional>

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::scripting::python {
	template<typename Ret, typename... Args>
	void register_function(const entt::registry & r, const char * name, const std::function<Ret(Args...)> & func) noexcept;
}

#include "register_function.inl"