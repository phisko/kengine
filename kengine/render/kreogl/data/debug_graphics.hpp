#pragma once

// stl
#include <vector>

// kreogl
#include "kreogl/debug_element.hpp"

namespace kengine::data {
	//! putils reflect name
	struct kreogl_debug_graphics {
		std::vector<kreogl::debug_element> elements;
	};
}

#include "kreogl_debug_graphics.rpp"