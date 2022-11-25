#pragma once

// stl
#include <vector>

// kreogl
#include "kreogl/debug_element.hpp"

namespace kengine::data {
	struct kreogl_debug_graphics {
		std::vector<kreogl::debug_element> elements;
	};
}