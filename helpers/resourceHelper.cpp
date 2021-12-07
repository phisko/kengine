#include <filesystem>
#include "kengine.hpp"
#include "resourceHelper.hpp"
#include "assertHelper.hpp"
#include "logHelper.hpp"

#include "data/ModelComponent.hpp"

#include "stb_image.h"

namespace kengine::resourceHelper {
	bool isSupportedTextureFormat(const char * file) noexcept {
		int x, y, comp;
		return stbi_info(file, &x, &y, &comp);
	}


	EntityID createTextureModel(const char * file) noexcept {
		for (const auto [e, model, textureData] : entities.with<ModelComponent, TextureDataComponent>())
			if (model.file == file) {
				kengine_logf(Log, "resourceHelper/loadTexture", "Found existing model %zu for %s", e.id, file);
				return e.id;
			}

		auto textureData = loadTexture(file);
		if (!textureData)
			return INVALID_ID;

		EntityID modelID;
		entities += [&](Entity & e) noexcept {
			kengine_logf(Log, "resourceHelper/loadTexture", "Created model %zu for %s", e.id, file);
			modelID = e.id;
			e.attach<ModelComponent>().file = file;
			e += std::move(*textureData);
		};

		return modelID;
	}

	std::optional<TextureDataComponent> loadTexture(const char * file) noexcept {
		TextureDataComponent textureData;

		textureData.data = stbi_load(file, &textureData.width, &textureData.height, &textureData.components, 0);
		if (textureData.data == nullptr) {
			kengine_assert_failed("Error loading texture: ", file);
			return std::nullopt;
		}

		textureData.free = stbi_image_free;

		return textureData;
	}

	EntityID createTextureModel(void * data, size_t width, size_t height) noexcept {
		struct LoadedFromMemory {
			const void * data = nullptr;
		};

		for (const auto [e, loaded] : entities.with<LoadedFromMemory>())
			if (loaded.data == data)
				return e.id;

		auto textureData = loadTexture(data, width, height);
		if (!textureData)
			return INVALID_ID;

		EntityID modelID;
		entities += [&](Entity & e) noexcept {
			modelID = e.id;
			e.attach<LoadedFromMemory>().data = data;
			e += std::move(*textureData);
		};

		return modelID;
	}

	std::optional<TextureDataComponent> loadTexture(void * data, size_t width, size_t height) noexcept {
		static constexpr auto expectedChannels = 4;

		TextureDataComponent textureData;
		if (height == 0) {
			textureData.data = stbi_load_from_memory((const unsigned char *)data, (int)width, &textureData.width, &textureData.height, &textureData.components, expectedChannels);
			if (textureData.data == nullptr) {
				kengine_assert_failed("Error loading texture from memory");
				return std::nullopt;
			}
			textureData.free = stbi_image_free;
		}
		else {
			textureData.data = data;
			textureData.width = (int)width;
			textureData.height = (int)height;
			textureData.components = expectedChannels;
		}

		return textureData;
	}


}