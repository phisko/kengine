#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	KENGINE_CORE_EXPORT void createCommandLineEntity(entt::registry & r, int argc, const char ** argv) noexcept;

    template<typename T>
    T parseCommandLine(const entt::registry & r) noexcept;
}

#include "commandLineHelper.inl"