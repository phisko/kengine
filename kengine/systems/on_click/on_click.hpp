#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_ONCLICK_EXPORT void add_on_click(entt::registry & r) noexcept;
}