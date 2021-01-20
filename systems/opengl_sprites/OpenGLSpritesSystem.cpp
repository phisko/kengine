#include "OpenGLSpritesSystem.hpp"

#include "kengine.hpp"
#include "SpritesShader.hpp"
#include "data/InstanceComponent.hpp"
#include "data/ShaderComponent.hpp"
#include "data/GraphicsComponent.hpp"

#include "functions/OnEntityCreated.hpp"
#include "helpers/resourceHelper.hpp"

namespace kengine {
	EntityCreator * OpenGLSpritesSystem() noexcept {
		struct impl {
			static void init(Entity & e) noexcept {
				e += functions::OnEntityCreated{ onEntityCreated };
				e += SystemSpecificShaderComponent<putils::gl::Program>{ std::make_unique<SpritesShader>() };
				e += GBufferShaderComponent{};
			}

			static void onEntityCreated(Entity & e) noexcept {
				if (!e.has<GraphicsComponent>())
					return;

				const auto & file = e.get<GraphicsComponent>().appearance;
				const auto model = resourceHelper::loadTexture(file);
				if (model == INVALID_ID)
					return;
				e += InstanceComponent{ model };
			}
		};

		return impl::init;
	}
}
