#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::scripting {
	template<typename T, typename Func, typename Func2>
	void register_component(entt::registry & r, Func && register_entity_member, Func2 && register_function) noexcept;
}

#include "register_component.inl"