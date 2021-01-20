#include <filesystem>
#include "kengine.hpp"
#include "resourceHelper.hpp"
#include "assertHelper.hpp"

#include "data/ModelComponent.hpp"
#include "data/TextureDataComponent.hpp"

#include "stb_image.h"
#include "concat.hpp"

namespace kengine::resourceHelper {
	EntityID loadTexture(const char * file) noexcept {
		for (const auto [e, model, textureData] : entities.with<ModelComponent, TextureDataComponent>())
			if (model.file == file)
				return e.id;

		TextureDataComponent textureData;
		textureData.data = stbi_load(file, &textureData.width, &textureData.height, &textureData.components, 0);
		if (textureData.data == nullptr)
			return INVALID_ID;
		textureData.free = stbi_image_free;

		EntityID modelID;
		entities += [&](Entity & e) noexcept {
			modelID = e.id;
			e.attach<ModelComponent>().file = file;
			e += std::move(textureData);
		};

		return modelID;
	}

	EntityID loadTexture(void * data, size_t width, size_t height) noexcept {
		struct LoadedFromMemory {
			const void * data = nullptr;
		};

		for (const auto [e, loaded] : entities.with<LoadedFromMemory>())
			if (loaded.data == data)
				return e.id;

		static constexpr auto expectedChannels = 4;

		TextureDataComponent textureData;
		if (height == 0) {
			textureData.data = stbi_load_from_memory((const unsigned char *)data, (int)width, &textureData.width, &textureData.height, &textureData.components, expectedChannels);
			if (textureData.data == nullptr) {
				kengine_assert_failed("Error loading texture from memory");
				return INVALID_ID;
			}
			textureData.free = stbi_image_free;
		}
		else {
			textureData.data = data;
			textureData.width = (int)width;
			textureData.height = (int)height;
			textureData.components = expectedChannels;
		}

		EntityID modelID;
		entities += [&](Entity & e) noexcept {
			modelID = e.id;
			e.attach<LoadedFromMemory>().data = data;
			e += std::move(textureData);
		};

		return modelID;
	}
}