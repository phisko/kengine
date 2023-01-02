#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename... Comps>
	void register_attach_to(entt::registry & r) noexcept;
}

#include "register_attach_to.inl"