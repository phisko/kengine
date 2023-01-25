#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_JSON_SCENE_LOADER_EXPORT entt::entity add_json_scene_loader(entt::registry & r) noexcept;
}