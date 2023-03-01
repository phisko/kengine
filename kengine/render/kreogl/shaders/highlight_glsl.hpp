#pragma once

// kreogl
#include "kreogl/impl/shaders/uniform.hpp"

namespace kengine::render::kreogl {
	struct highlight_glsl {
		static const char * frag;

		::kreogl::uniform<int> gposition{ "gposition" };
		::kreogl::uniform<int> guser_data{ "guser_data" };

		::kreogl::uniform<::glm::vec3> view_pos{ "view_pos" };
		::kreogl::uniform<::glm::vec2> screen_size{ "screen_size" };
		::kreogl::uniform<float> entity_id{ "entity_id" };
		::kreogl::uniform<::glm::vec4> highlight_color{ "highlight_color" };
		::kreogl::uniform<float> intensity{ "intensity" };
	};
}