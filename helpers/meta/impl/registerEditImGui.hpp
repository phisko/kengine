#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ... Comps>
	void registerEditImGui(entt::registry & r) noexcept;
}

#include "registerEditImGui.inl"