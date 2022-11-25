#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	KENGINE_CORE_EXPORT void create_command_line_entity(entt::registry & r, int argc, const char ** argv) noexcept;

    template<typename T>
    T parse_command_line(const entt::registry & r) noexcept;
}

#include "command_line_helper.inl"