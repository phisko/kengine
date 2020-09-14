#include "OpenGLSpritesSystem.hpp"

#include "EntityManager.hpp"
#include "SpritesShader.hpp"
#include "data/InstanceComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/TextureModelComponent.hpp"
#include "data/TextureDataComponent.hpp"
#include "data/ShaderComponent.hpp"
#include "data/GraphicsComponent.hpp"
#include "data/SpriteComponent.hpp"

#include "functions/OnEntityCreated.hpp"

#include "stb_image.h"

namespace kengine {
	static EntityManager * g_em;

#pragma region declarations
	static void onEntityCreated(Entity & e);
#pragma endregion
	EntityCreator * OpenGLSpritesSystem(EntityManager & em) {
		g_em = &em;

		return [](Entity & e) {
			e += functions::OnEntityCreated{ onEntityCreated };
			e += makeGBufferShaderComponent<SpritesShader>(*g_em);
		};
	}

	void onEntityCreated(Entity & e) {
		if (!e.has<GraphicsComponent>())
			return;

		if (!e.has<SpriteComponent2D>() && !e.has<SpriteComponent3D>())
			return;

		if (e.has<InstanceComponent>() && e.get<InstanceComponent>().model != Entity::INVALID_ID)
			return;

		const auto & file = e.get<GraphicsComponent>().appearance;

		for (const auto &[model, comp] : g_em->getEntities<TextureModelComponent<putils::gl::Texture>>())
			if (comp.file == file) {
				e += InstanceComponent{ model.id };
				return;
			}

		int width, height, components;
		const auto data = stbi_load(file.c_str(), &width, &height, &components, 0);
		if (data == nullptr)
			return; // Not supported image type

		*g_em += [&](Entity & model) {
			e += InstanceComponent{ model.id };

			auto & comp = model.attach<TextureModelComponent<putils::gl::Texture>>();
			comp.file = file;

			TextureDataComponent<putils::gl::Texture> textureLoader; {
				textureLoader.textureID = &comp.texture;

				textureLoader.data = data;
				textureLoader.width = width;
				textureLoader.height = height;
				textureLoader.components = components;

				textureLoader.free = stbi_image_free;
			}
			model += textureLoader;
		};
	}
}
