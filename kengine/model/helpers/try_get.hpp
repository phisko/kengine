#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine
#include "kengine/instance/data/instance.hpp"

namespace kengine::instance {
	template<typename Comp>
	const Comp * try_get_model(entt::const_handle instance_entity) noexcept;

	template<typename Comp>
	const Comp * try_get_model(const entt::registry & r, const instance & instance) noexcept;
}

#include "try_get_model.inl"