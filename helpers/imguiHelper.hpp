#pragma once

#include "kengine.hpp"

namespace kengine::imguiHelper {
	KENGINE_CORE_EXPORT void displayEntity(const Entity & e) noexcept;
	KENGINE_CORE_EXPORT void displayEntityAndModel(const Entity & e) noexcept;
	KENGINE_CORE_EXPORT void editEntity(Entity & e) noexcept;
	KENGINE_CORE_EXPORT void editEntityAndModel(Entity & e) noexcept;
	KENGINE_CORE_EXPORT float getScale() noexcept;
}