#pragma once

#include "data/LightComponent.hpp"

#include "putils/opengl/Program.hpp"
#include "putils/opengl/RAII.hpp"

namespace kengine::opengl {
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
}