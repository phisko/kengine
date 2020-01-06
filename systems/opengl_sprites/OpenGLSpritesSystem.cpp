#include "OpenGLSpritesSystem.hpp"

#include "EntityManager.hpp"
#include "SpritesShader.hpp"
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

	// declarations
	static void onEntityCreated(Entity & e);
	//
	EntityCreator * OpenGLSpritesSystem(EntityManager & em) {
		g_em = &em;

		em += [&](Entity & e) {
			e += kengine::makeGBufferShaderComponent<SpritesShader>(em);
		};

		return [](Entity & e) {
			e += functions::OnEntityCreated{ onEntityCreated };
		};
	}

	void onEntityCreated(Entity & e) {
		if (!e.has<GraphicsComponent>())
			return;

		if (!e.has<SpriteComponent2D>() && !e.has<SpriteComponent3D>())
			return;

		auto & graphics = e.get<GraphicsComponent>();
		const auto & file = graphics.appearance;

		for (const auto &[e, model] : g_em->getEntities<TextureModelComponent>())
			if (model.file == file) {
				graphics.model = e.id;
				return;
			}

		int width, height, components;
		const auto data = stbi_load(file.c_str(), &width, &height, &components, 0);
		if (data == nullptr)
			return; // Not supported image type

		*g_em += [&](Entity & e) {
			graphics.model = e.id;

			auto & comp = e.attach<TextureModelComponent>();
			comp.file = file;

			TextureDataComponent textureLoader; {
				textureLoader.textureID = &comp.texture;

				textureLoader.data = data;
				textureLoader.width = width;
				textureLoader.height = height;
				textureLoader.components = components;

				textureLoader.free = stbi_image_free;
			} e += textureLoader;
		};
	}
}
