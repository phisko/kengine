#include "OpenGLSpritesSystem.hpp"

#include "kengine.hpp"
#include "SpritesShader.hpp"
#include "data/InstanceComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/TextureDataComponent.hpp"
#include "data/ShaderComponent.hpp"
#include "data/GraphicsComponent.hpp"
#include "data/SpriteComponent.hpp"

#include "functions/OnEntityCreated.hpp"

#include "stb_image.h"

namespace kengine {
	EntityCreator * OpenGLSpritesSystem() noexcept {
		struct impl {
			static void init(Entity & e) noexcept {
				e += functions::OnEntityCreated{ onEntityCreated };
				e += opengl::ShaderComponent{ std::make_unique<SpritesShader>() };
				e += opengl::GBufferShaderComponent{};
			}

			static void onEntityCreated(Entity & e) noexcept {
				if (!e.has<GraphicsComponent>())
					return;

				if (!e.has<SpriteComponent2D>() && !e.has<SpriteComponent3D>())
					return;

				if (e.has<InstanceComponent>() && e.get<InstanceComponent>().model != INVALID_ID)
					return;

				const auto & file = e.get<GraphicsComponent>().appearance;

				for (const auto & [model, comp] : entities.with<ModelComponent>())
					if (comp.file == file) {
						e += InstanceComponent{ model.id };
						return;
					}

				int width, height, components;
				const auto data = stbi_load(file.c_str(), &width, &height, &components, 0);
				if (data == nullptr)
					return; // Not supported image type

				entities += [&](Entity & model) noexcept {
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

		return [](Entity & e) {
			impl::init(e);
		};
	}
}
