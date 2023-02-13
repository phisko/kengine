#pragma once

// stl
#include <type_traits>

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename Meta, typename... Comps> // Func: prototype of Meta, with an additional putils::meta::type<T> as first parameter
	void register_meta_component_implementation(entt::registry & r) noexcept;

	template<typename Meta, typename T>
	struct meta_component_implementation : std::true_type {};
}

#include "register_meta_component_implementation.inl"