#pragma once

// kreogl
#include "kreogl/impl/shaders/Uniform.hpp"

struct HighlightGLSL {
	static const char * frag;

	kreogl::Uniform<int> gposition{ "gposition" };
	kreogl::Uniform<int> guserData{ "guserData" };

	kreogl::Uniform<glm::vec3> viewPos{ "viewPos" };
	kreogl::Uniform<glm::vec2> screenSize{ "screenSize" };
	kreogl::Uniform<float> entityID{ "entityID" };
	kreogl::Uniform<glm::vec4> highlightColor{ "highlightColor" };
	kreogl::Uniform<float> intensity{ "intensity" };
};