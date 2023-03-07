#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine
#include "kengine/model/data/instance.hpp"

namespace kengine::model {
	template<typename Comp>
	const Comp & get(entt::const_handle instance) noexcept;

	template<typename Comp>
	const Comp & get(const entt::registry & r, const instance & instance) noexcept;
}

#include "get.inl"