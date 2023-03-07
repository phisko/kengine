#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine
#include "kengine/model/data/instance.hpp"

namespace kengine::model {
	template<typename Comp>
	bool has(entt::const_handle instance_entity) noexcept;

	template<typename Comp>
	bool has(const entt::registry & r, const instance & instance) noexcept;
}

#include "has.inl"