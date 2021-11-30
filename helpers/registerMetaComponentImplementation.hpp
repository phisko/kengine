#pragma once

namespace kengine {
	template<typename Meta, typename ... Comps, typename Func> // Func: prototype of Meta, with an additional putils::meta::type<T> as first parameter
	void registerMetaComponentImplementation(Func && f) noexcept;
}

#include "registerMetaComponentImplementation.inl"