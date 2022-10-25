#pragma once

#ifdef KENGINE_PYTHON

#include "scriptLanguageHelper.hpp"

namespace kengine::pythonHelper {
	template<typename ... Types>
	void registerTypes() noexcept;

	template<typename ... Comps>
	void registerComponents() noexcept;

	template<typename Ret, typename ...Args>
	void registerFunction(const char * name, const scriptLanguageHelper::function<Ret(Args...)> & func) noexcept;
}

#include "pythonHelper.inl"

#endif