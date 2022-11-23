#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ... Comps>
	void registerDisplayImGui(entt::registry & r) noexcept;
}

#include "registerDisplayImGui.inl"