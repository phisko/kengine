#include "OpenGLSpritesSystem.hpp"

#include "EntityManager.hpp"
#include "SpritesShader.hpp"
#include "data/InstanceComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/TextureDataComponent.hpp"
#include "data/ShaderComponent.hpp"
#include "data/GraphicsComponent.hpp"
#include "data/SpriteComponent.hpp"

#include "functions/OnEntityCreated.hpp"

#include "stb_image.h"

namespace kengine::opengl_sprites {
	struct impl {
		static inline EntityManager * em;

		static void init(Entity & e) {
			e += functions::OnEntityCreated{ onEntityCreated };
			e += opengl::ShaderComponent{ std::make_unique<SpritesShader>(*em) };
			e += opengl::GBufferShaderComponent{};
		}

		static void onEntityCreated(Entity & e) {
			if (!e.has<GraphicsComponent>())
				return;

			if (!e.has<SpriteComponent2D>() && !e.has<SpriteComponent3D>())
				return;

			if (e.has<InstanceComponent>() && e.get<InstanceComponent>().model != Entity::INVALID_ID)
				return;

			const auto & file = e.get<GraphicsComponent>().appearance;

			for (const auto & [model, comp] : em->getEntities<ModelComponent>())
				if (comp.file == file) {
					e += InstanceComponent{ model.id };
					return;
				}

			int width, height, components;
			const auto data = stbi_load(file.c_str(), &width, &height, &components, 0);
			if (data == nullptr)
				return; // Not supported image type

			*em += [&](Entity & model) {
				e += InstanceComponent{ model.id };

				TextureDataComponent textureData;
				textureData.data = data;
				textureData.width = width;
				textureData.height = height;
				textureData.components = components;
				textureData.free = stbi_image_free;
				model += std::move(textureData);
			};
		}
	};
}

namespace kengine {
	EntityCreator * OpenGLSpritesSystem(EntityManager & em) {
		opengl_sprites::impl::em = &em;
		return [](Entity & e) {
			opengl_sprites::impl::init(e);
		};
	}
}
