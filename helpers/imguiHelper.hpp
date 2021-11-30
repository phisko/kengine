#pragma once

#include "kengine.hpp"

namespace kengine::imguiHelper {
	void displayEntity(const Entity & e) noexcept;
	void displayEntityAndModel(const Entity & e) noexcept;
	void editEntity(Entity & e) noexcept;
	void editEntityAndModel(Entity & e) noexcept;
	float getScale() noexcept;
}