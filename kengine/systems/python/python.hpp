#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_PYTHON_EXPORT void add_python(entt::registry & r) noexcept;
}