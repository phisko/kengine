#pragma once

#include <memory>

#include "putils/opengl/Program.hpp"
#include "putils/opengl/RAII.hpp"
#include "data/LightComponent.hpp"

namespace kengine {
	class Entity;

	namespace opengl {
		struct ShaderComponent {
			std::unique_ptr<putils::gl::Program> shader;
			bool enabled = true;
		};

		struct GBufferShaderComponent {};
		struct LightingShaderComponent {};
		struct PostLightingShaderComponent {};
		struct PostProcessShaderComponent {};

		struct DepthMapComponent {
			putils::gl::FrameBuffer fbo;
			putils::gl::Texture texture;
			int size = -1;
		};

		struct CSMComponent {
			putils::gl::FrameBuffer fbo;
			putils::gl::Texture textures[KENGINE_MAX_CSM_COUNT];
			int size = -1;
		};

		struct DepthCubeComponent {
			putils::gl::FrameBuffer fbo;
			putils::gl::Texture texture;
			int size = -1;
		};

		struct ShadowMapShaderComponent {};
		struct ShadowCubeShaderComponent {};
	}
}
