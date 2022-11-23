#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
    template<typename ... Comps>
    void registerAttributes(entt::registry & r) noexcept;
}

#include "registerAttributes.inl"