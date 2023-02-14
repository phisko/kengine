#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	KENGINE_CORE_EXPORT void pre_register_all_types(entt::registry & destination_registry, const entt::registry * main_registry = nullptr) noexcept;
	KENGINE_CORE_EXPORT void register_all_types(entt::registry & destination_registry, const entt::registry * main_registry = nullptr) noexcept;
}