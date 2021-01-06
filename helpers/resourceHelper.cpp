#include <filesystem>
#include "kengine.hpp"
#include "resourceHelper.hpp"
#include "assertHelper.hpp"

#include "data/ModelComponent.hpp"
#include "data/TextureDataComponent.hpp"

#ifdef KENGINE_OPENGL
#include "opengl/RAII.hpp"
#endif

#ifdef KENGINE_VULKAN
#include "vulkan/Image.hpp"
#endif

#include "stb_image.h"

namespace kengine::resourceHelper {
	EntityID loadTexture(const char * file) noexcept {
		const std::filesystem::path path = file;

		const auto directory = path.parent_path();
		const auto filename = path.filename();

		for (const auto [e, model] : entities.with<ModelComponent>())
			if (model.file == file)
				return e.id;

		EntityID modelID;

		entities += [&](Entity & e) noexcept {
			modelID = e.id;

			e.attach<ModelComponent>().file = file;

			auto & textureData = e.attach<TextureDataComponent>();
			textureData.data = stbi_load(file, &textureData.width, &textureData.height, &textureData.components, 0);
			kengine_assert_with_message(textureData.data != nullptr, putils::string<1024>("Error loading texture file [%s]", file).c_str());
			textureData.free = stbi_image_free;
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

		EntityID modelID;

		entities += [&](Entity & e) noexcept {
			modelID = e.id;
			e.attach<LoadedFromMemory>().data = data;

			static constexpr auto expectedChannels = 4;

			auto & textureData = e.attach<TextureDataComponent>();
			if (height == 0) { // Compressed format
				textureData.data = stbi_load_from_memory((const unsigned char *)data, (int)width, &textureData.width, &textureData.height, &textureData.components, expectedChannels);
				kengine_assert_with_message(textureData.data != nullptr, "Error loading texture from memory");
				textureData.free = stbi_image_free;
			}
			else {
				textureData.data = data;
				textureData.width = (int)width;
				textureData.height = (int)height;
				textureData.components = expectedChannels;
			}
		};

		return modelID;
	}
}