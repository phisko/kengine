#pragma once

#include "Entity.hpp"
#include "data/TextureDataComponent.hpp"

namespace kengine::resourceHelper {
	bool isSupportedTextureFormat(const char * file) noexcept;

	EntityID createTextureModel(const char * file) noexcept;
	std::optional<TextureDataComponent> loadTexture(const char * file) noexcept;

	EntityID createTextureModel(void * data, size_t width, size_t height) noexcept;
	std::optional<TextureDataComponent> loadTexture(void * data, size_t width, size_t height) noexcept;
}