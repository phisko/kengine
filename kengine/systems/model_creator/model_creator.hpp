#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_MODEL_CREATOR_EXPORT entt::entity add_model_creator(entt::registry & r) noexcept;
}