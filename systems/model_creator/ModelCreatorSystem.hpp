#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	KENGINE_MODEL_CREATOR_EXPORT void addModelCreatorSystem(entt::registry & r) noexcept;
}