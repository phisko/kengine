#pragma once

#include "kengine.hpp"

namespace kengine::typeHelper {
    template <typename T>
    Entity getTypeEntity() noexcept;
}

#include "typeHelper.inl"