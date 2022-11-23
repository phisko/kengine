#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::typeHelper {
    template <typename T>
    entt::entity getTypeEntity(entt::registry & r) noexcept;
}

#include "typeHelper.inl"