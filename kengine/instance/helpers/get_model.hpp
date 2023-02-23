#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine
#include "kengine/instance/data/instance.hpp"

namespace kengine::instance {
	template<typename Comp>
	const Comp & get_model(entt::const_handle instance) noexcept;

	template<typename Comp>
	const Comp & get_model(const entt::registry & r, const instance & instance) noexcept;
}

#include "get_model.inl"