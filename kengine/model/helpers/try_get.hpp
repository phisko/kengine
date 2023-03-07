#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine
#include "kengine/model/data/instance.hpp"

namespace kengine::model {
	template<typename Comp>
	const Comp * try_get(entt::const_handle instance_entity) noexcept;

	template<typename Comp>
	const Comp * try_get(const entt::registry & r, const instance & instance) noexcept;
}

#include "try_get.inl"