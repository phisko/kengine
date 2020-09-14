#include <filesystem>
#include "resourceHelper.hpp"
#include "assertHelper.hpp"

#include "data/ModelComponent.hpp"
#include "data/TextureModelComponent.hpp"
#include "data/TextureDataComponent.hpp"

#include "stb_image.h"

namespace kengine::resourceHelper {
	Entity::ID loadTexture(EntityManager & em, const char * file) {
		const std::filesystem::path path = file;

		const auto directory = path.parent_path();
		const auto filename = path.filename();

		for (const auto & [e, model] : em.getEntities<ModelComponent>())
			if (model.file == file)
				return e.id;

		Entity::ID modelID;

		em += [&](Entity & e) {
			modelID = e.id;

			auto & comp = e.attach<TextureModelComponent>();
			comp.file = file;

			TextureDataComponent textureLoader;
			textureLoader.textureID = &comp.texture.get();
			textureLoader.data = stbi_load(file, &textureLoader.width, &textureLoader.height, &textureLoader.components, 0);
			kengine_assert_with_message(em, textureLoader.data != nullptr, putils::string<1024>("Error loading texture file [%s]", file).c_str());
			textureLoader.free = stbi_image_free;

			e += textureLoader;
		};

		return modelID;
	}

	Entity::ID loadTexture(EntityManager & em, void * data, size_t width, size_t height) {
		struct LoadedFromMemory {
			const void * data = nullptr;
		};

		for (const auto & [e, loaded] : em.getEntities<LoadedFromMemory>())
			if (loaded.data == data)
				return e.id;

		Entity::ID modelID;

		em += [&](Entity & e) {
			modelID = e.id;
			e.attach<LoadedFromMemory>().data = data;

			static constexpr auto expectedChannels = 4;
			auto & comp = e.attach<TextureModelComponent>();
			comp.file = "loaded from memory";

			TextureDataComponent textureLoader;
			textureLoader.textureID = &comp.texture.get();
			if (height == 0) { // Compressed format
				textureLoader.data = stbi_load_from_memory((const unsigned char *)data, (int)width, &textureLoader.width, &textureLoader.height, &textureLoader.components, expectedChannels);
				kengine_assert_with_message(em, textureLoader.data != nullptr, "Error loading texture from memory");
				textureLoader.free = stbi_image_free;
			}
			else {
				textureLoader.data = data;
				textureLoader.width = (int)width;
				textureLoader.height = (int)height;
				textureLoader.components = expectedChannels;
			}

			e += textureLoader;
		};

		return modelID;
	}
}