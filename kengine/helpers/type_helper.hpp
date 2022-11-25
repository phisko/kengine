#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::type_helper {
    template <typename T>
    entt::entity get_type_entity(entt::registry & r) noexcept;
}

#include "type_helper.inl"