#pragma once

// stl
#include <vector>

// kreogl
#include "kreogl/debug_element.hpp"

namespace kengine::render::kreogl {
	//! putils reflect name
	//! class_name: kreogl_debug_graphics
	struct debug_graphics {
		std::vector<::kreogl::debug_element> elements;
	};
}

#include "debug_graphics.rpp"