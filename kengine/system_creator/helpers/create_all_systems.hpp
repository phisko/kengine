#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::system_creator {
	KENGINE_SYSTEM_CREATOR_EXPORT void create_all_systems(entt::registry & destination_registry, const entt::registry * main_registry = nullptr) noexcept;
}