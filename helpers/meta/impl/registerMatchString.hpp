#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ... Comps>
	void registerMatchString(entt::registry & r) noexcept;
}

#include "registerMatchString.inl"
