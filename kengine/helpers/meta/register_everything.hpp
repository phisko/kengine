#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<bool IsComponent, typename... Comps>
	void register_everything(entt::registry & r) noexcept;
}

#include "register_everything.inl"