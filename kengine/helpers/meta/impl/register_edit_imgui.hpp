#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename... Comps>
	void register_edit_imgui(entt::registry & r) noexcept;
}

#include "register_edit_imgui.inl"