#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine
#include "kengine/instance/data/instance.hpp"

namespace kengine::instance {
	template<typename Comp>
	bool model_has(entt::const_handle instance_entity) noexcept;

	template<typename Comp>
	bool model_has(const entt::registry & r, const instance & instance) noexcept;
}

#include "model_has.inl"