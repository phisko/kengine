#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::meta {
	template<typename Meta, typename... Comps> // Func: prototype of Meta, with an additional putils::meta::type<T> as first parameter
	void register_meta_component_implementation(entt::registry & r) noexcept;
}

#include "register_meta_component_implementation.inl"