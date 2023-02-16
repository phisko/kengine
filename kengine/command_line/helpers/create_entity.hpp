#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::command_line {
	KENGINE_COMMAND_LINE_EXPORT void create_entity(entt::registry & r, int argc, const char ** argv) noexcept;
}