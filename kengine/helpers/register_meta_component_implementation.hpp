#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename Meta, typename ... Comps, typename Func> // Func: prototype of Meta, with an additional putils::meta::type<T> as first parameter
	void register_meta_component_implementation(entt::registry & r, Func && f) noexcept;

    template<typename Meta, template<typename T> typename Predicate, typename ... Comps, typename Func>
    void register_meta_component_implementation_with_predicate(entt::registry & r, Func && f) noexcept;
}

#include "register_meta_component_implementation.inl"