#include "OpenGLSpritesSystem.hpp"

#include "SpritesShader.hpp"
#include "data/ShaderComponent.hpp"
#include "data/ModelComponent.hpp"

#include "functions/OnEntityCreated.hpp"
#include "helpers/resourceHelper.hpp"
#include "helpers/logHelper.hpp"

namespace kengine {
	EntityCreator * OpenGLSpritesSystem() noexcept {
		struct impl {
			static void init(Entity & e) noexcept {
				kengine_log(Log, "Init", "OpenGLSpritesSystem");
				e += functions::OnEntityCreated{ onEntityCreated };
				e += SystemSpecificShaderComponent<putils::gl::Program>{ std::make_unique<SpritesShader>() };
				e += GBufferShaderComponent{};
			}

			static void onEntityCreated(Entity & e) noexcept {
				if (e.has<TextureDataComponent>())
					return;

				const auto model = e.tryGet<ModelComponent>();
				if (model == nullptr)
					return;

				const auto & file = e.get<ModelComponent>().file;
				if (!resourceHelper::isSupportedTextureFormat(file.c_str()))
					return;

				kengine_logf(Log, "OpenGLSpritesSystem", "Loading texture %s for %zu", file.c_str(), e.id);
				auto textureData = resourceHelper::loadTexture(file);
				if (textureData)
					e += std::move(*textureData);
			}
		};

		return impl::init;
	}
}
