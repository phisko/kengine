#pragma once

#include "Entity.hpp"

namespace kengine::resourceHelper {
	EntityID loadTexture(const char * file) noexcept;
	EntityID loadTexture(void * data, size_t width, size_t height) noexcept;
}