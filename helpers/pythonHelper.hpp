#pragma once

#ifdef KENGINE_PYTHON

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/scriptLanguageHelper.hpp"

namespace kengine::pythonHelper {
	template<typename ... Types>
	void registerTypes(const entt::registry & r) noexcept;

	template<typename ... Comps>
	void registerComponents(entt::registry & r) noexcept;

	template<typename Ret, typename ...Args>
	void registerFunction(const entt::registry & r, const char * name, const scriptLanguageHelper::function<Ret(Args...)> & func) noexcept;
}

#include "pythonHelper.inl"

#endif