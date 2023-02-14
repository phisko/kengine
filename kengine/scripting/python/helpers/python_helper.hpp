#pragma once

#ifdef KENGINE_PYTHON

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/script_language_helper.hpp"

namespace kengine::python_helper {
	template<bool IsComponent, typename... Types>
	void register_types(entt::registry & r) noexcept;

	template<typename Ret, typename... Args>
	void register_function(const entt::registry & r, const char * name, const script_language_helper::function<Ret(Args...)> & func) noexcept;
}

#include "python_helper.inl"

#endif