#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::meta {
	template<typename T>
	entt::entity get_type_entity(entt::registry & r) noexcept;
}

#include "get_type_entity.inl"