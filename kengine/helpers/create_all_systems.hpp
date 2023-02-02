#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	KENGINE_CORE_EXPORT void create_all_systems(entt::registry & destination_registry, const entt::registry * main_registry = nullptr) noexcept;
}