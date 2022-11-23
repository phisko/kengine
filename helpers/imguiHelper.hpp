#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::imguiHelper {
	KENGINE_CORE_EXPORT void displayEntity(entt::const_handle e) noexcept;
	KENGINE_CORE_EXPORT void displayEntityAndModel(entt::const_handle e) noexcept;
	KENGINE_CORE_EXPORT void editEntity(entt::handle e) noexcept;
	KENGINE_CORE_EXPORT void editEntityAndModel(entt::handle e) noexcept;
	KENGINE_CORE_EXPORT float getScale(const entt::registry & r) noexcept;
}