#pragma once

namespace kengine {
	template<typename ... Comps>
	void registerComponents() noexcept;

	template<typename ...Types>
	void registerTypes() noexcept;

	template<typename F>
	void registerFunction(const char * name, F && func) noexcept;
}

#include "registerTypeHelper.inl"
