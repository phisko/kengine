#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::command_line {
	template<typename T>
	T parse(const entt::registry & r) noexcept;
}

#include "parse.inl"